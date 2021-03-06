/*******************************************************************************
 * Copyright (c) 2013 Alexandre Arsenault.
 *
 * This file is part of axLibrary.
 *
 * axLibrary is free or commercial software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 or any later version of the
 * License or use a commercial axLibrary License.
 *
 * axLibrary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with axLibrary. If not, see <http://www.gnu.org/licenses/>.
 *
 * To release a closed-source product which uses axLibrary, commercial
 * licenses are available, email alx.arsenault@gmail.com for more information.
 ******************************************************************************/
#include "axWindowTree.h"
#include "axWindow.h"
#include "axApp.h"
#include "axCore.h"
#include "axMath.h"

axWindowNode::axWindowNode(axWindow* win)
{
	window = win;
}

axWindowNode::axWindowNode()
{
}

axWindow* axWindowNode::GetWindow()
{
	return window;
}

void axWindowNode::SetWindow(axWindow* win)
{
	window = win;
}

void axWindowNode::AddWindow(axWindow* win)
{
	_childNodes.push_back(new axWindowNode(win));
}

axWindowNode* axWindowNode::Get(axWindow* win)
{
	for (axWindowNode* it : _childNodes)
	{
		if (it->window == win)
		{
			return it;
		}
	}
	return nullptr;
}

vector<axWindowNode*>& axWindowNode::GetChild()
{
	return _childNodes;
}


void BeforeDrawing(axWindow* win)
{
    if(win->IsBlockDrawing())
    {
        axMatrix4 mview;
        mview.Identity().Load();
        
        axRect abs_rect = win->GetAbsoluteRect();
        axRect shown_rect = win->GetShownRect();
        
        double delta_size_x = shown_rect.size.x - abs_rect.size.x;
        double delta_size_y = shown_rect.size.y - abs_rect.size.y;
        
        double globalY = axApp::GetInstance()->GetCore()->GetGlobalSize().y;
        double sumY = (abs_rect.position.y + shown_rect.position.y +
                       abs_rect.size.y + delta_size_y);
        
        glScissor(abs_rect.position.x + shown_rect.position.x - 1,
                  globalY - sumY,
                  abs_rect.size.x + delta_size_x + 1,
                  abs_rect.size.y + delta_size_y + 1);
        
        glEnable(GL_SCISSOR_TEST);
    }
}

void EndDrawing(axWindow* win)
{
    if(win->IsBlockDrawing())
    {
        glDisable(GL_SCISSOR_TEST);
    }
}

void DrawWindow(axWindow* win)
{
    axMatrix4 mview;
    mview.Identity().Load();

    mview.Translate(win->GetAbsoluteRect().position -
                    win->GetScrollDecay()).Process();

    win->OnPaint();
}

void axWindowNode::DrawNode()
{
    if(window->IsShown())
    {
        axMatrix4 mview_before(GL_MODELVIEW_MATRIX);
        
        BeforeDrawing(window);
        DrawWindow(window);
        
        for(axWindowNode* it : _childNodes)
        {
            if(it->window != nullptr)
            {
                if(it->window->IsShown())
                {
                    axMatrix4 mview_child_before(GL_MODELVIEW_MATRIX);
                    
                    BeforeDrawing(it->window);
                    
                    DrawWindow(it->window);
                    it->DrawNode();
                    
                    axMatrix4 mview;
                    mview.Identity().Load();
                    mview.Translate(it->window->GetAbsoluteRect().position).Process();
                    it->window->OnPaintStatic();
                    
                    EndDrawing(it->window);
                    
                    mview_child_before.Load();
                }
            }
            
        }
        EndDrawing(window);
    }
}

//-----------------------------------------------------------------------------
// axWindowTree
//-----------------------------------------------------------------------------
axWindowTree::axWindowTree()
{
}

deque<axWindow*> axWindowTree::GetWindowParents(axWindow* win)
{
	deque<axWindow*> windows;

	while_not_null(win->GetParent())
	{
		win = win->GetParent();
		windows.push_front(win);
	}

	return windows;
}

axWindowNode* axWindowTree::FindWinNode(axWindow* win)
{
	deque<axWindow*> windows = GetWindowParents(win);

	if (windows.size() == 0)
	{
		return Get(win);
	}
	else
	{
		axWindowNode* parent = nullptr;

		// Find parent node.
		if_not_null(parent = Get(windows[0]))
		{
			windows.pop_front();

			for (axWindow* it : windows)
			{
				parent = parent->Get(it);
				if (parent == nullptr)
				{
					return nullptr;
				}
			}

			if (parent == nullptr)
			{
				return nullptr;
			}
			return parent->Get(win);
		}

		// Didn't find the window.
		return nullptr;
	}
}

void axWindowTree::AddWindow(axWindow* win)
{
	deque<axWindow*> windows = GetWindowParents(win);

	// If there's no node in the nodes vector 
	// then it must be the first one to be added
	// with nullptr parent.
	if (_nodes.size() == 0 && windows.size() == 0)
	{
        // First top level window.
		_nodes.push_back(new axWindowNode(win));
	}
	else if (windows.size() == 0)
	{
        // Second or more top level windows.
//        std::cout << "Add second top layer node." << std::endl;
		_nodes.push_back(new axWindowNode(win));
	}
	else
	{
		axWindowNode* node = FindWinNode(win->GetParent());
		if_not_null(node)
		{
			node->AddWindow(win);
		}
	}
}

void axWindowTree::DeleteWindow(axWindow* win)
{

}

vector<axWindowNode*> axWindowTree::GetMainNode()
{
	return _nodes;
}

axWindowNode* axWindowTree::Get(axWindow* win)
{
	for (axWindowNode* it : _nodes)
	{
		if (it->window == win)
		{
			return it;
		}
	}
	return nullptr;
}

axWindow* axWindowTree::FindMousePosition(const axPoint& pos)
{
	axWindowNode* node = nullptr;

	// Find first level window.
	for (axWindowNode* it : _nodes)
	{
		if (it->window->GetAbsoluteRect().IsPointInside(pos))
		{
			node = it;
		}
	}

	axWindowNode* n = node;
	if_not_null(n)
	{
		do
		{
			n = node;
			for (axWindowNode* it : n->GetChild())
			{
				if (it->window->GetAbsoluteRect().IsPointInside(pos) &&
                    it->window->IsShown() &&  it->window->IsSelectable())
				{
					node = it;
					break;
				}
			}
            
		} while (n != node);
	}

	if(node != nullptr && node->window->IsShown())
	{
		return node->window;
	}

	return nullptr;
}

void axWindowTree::DrawTree()
{
    for (axWindowNode* it : _nodes)
    {
        if(it != nullptr)
        {
            it->DrawNode();
        }
    }
}