#include "axCore.h"
#include "axMath.h"

axCore::axCore():
	// Members.
	_needToDraw(true),
	_popupNeedToDraw(true)
{
	_windowManager = new axManager();
    _windowManager->_managerName = std::string("WindowManager");
	 _popupManager = new axManager();
    _popupManager->_managerName = std::string("PopupManager");

	 //_lastDrawingTime = clock();
    
    _y_test = 0.0;
}

axManager* axCore::GetWindowManager()
{
	return _windowManager;
}

axManager* axCore::GetPopupManager()
{
	 return _popupManager;
}

void axCore::ResizeGLScene(const int& width, const int& height, double y)
{
	int h = height;

	// Prevent a divide by zero by.
	if (h == 0)
    {
        h = 1;
    }
    
    _y_test = y;
	_size = axSize(width, h);

	// Reset the current viewport.
	//--------------------------------------------------
	//--------------------------------------------------
#ifdef __APPLE__
	glViewport(0, _y_test, width, h);
#else
    glViewport(0, 0, width, h);
#endif // __APPLE__
	//--------------------------------------------------
	//--------------------------------------------------

    //axMatrix4 proj;
    //axOrtho2D(proj.Identity().GetData(), _size);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, _size.x, _size.y, 0);

	// Select the modelview matrix.
	glMatrixMode(GL_MODELVIEW);			
    
 /*   axMatrix4 mview;
    mview.Translate(axPoint(0, y)).Process();*/
    
	_needToDraw = true;
	_popupNeedToDraw = true;
    
    _windowManager->OnSize();

}

void axCore::DeletePopWindow()
{
 	KillPopGLWindow();

 	delete _popupManager;
 	_popupManager = nullptr;
}

int axCore::InitGL()										
{
	// Enable Smooth Shading.
	glShadeModel(GL_SMOOTH);				

	// Black Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);	

	// Depth Buffer Setup
	glClearDepth(1.0f);		

	// Enables Depth Testing
	glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND); // Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// The type of depth testing to do.
	glDepthFunc(GL_LEQUAL);				

	// Really nice perspective calculations.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	return true;										
}
//
//void axCore::ResizeGLScene(const int& width, const int& height)
//{
//	int h = height;
//
//	// Prevent a divide by zero by.
//	if (h == 0)	h = 1;
//	_size = axSize(width, h);
//
//	// Reset the current viewport.
//	glViewport(0, 0, width, h);
//
//	// Select the projection matrix.
//	glMatrixMode(GL_PROJECTION);
//
//	// Reset the projection matrix.
//	glLoadIdentity();
//
//	// Calculate the aspect ratio of the window.
//	gluPerspective(45.0f, (GLfloat)width / (GLfloat)h, 0.1f, 100.0f);
//
//	// Select the modelview matrix.
//	glMatrixMode(GL_MODELVIEW);
//
//	// Reset the modelview matrix.
//	glLoadIdentity();
//	_needToDraw = true;
//	_popupNeedToDraw = true;
//
//}

void axCore::ResizePopGLScene(const int& width, const int& height)
{
	int h = height;

	// Prevent a divide by zero by.
	if (h == 0)	h = 1;
		_popSize = axSize(width, h);

	// Reset the current viewport.
	glViewport(0, 0, width, h);

	// Select the projection matrix.
	glMatrixMode(GL_PROJECTION);

	// Reset the projection matrix.
	glLoadIdentity();

	// Calculate the aspect ratio of the window.
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)h, 0.1f, 100.0f);

	// Select the modelview matrix.
	glMatrixMode(GL_MODELVIEW);

	// Reset the modelview matrix.
	glLoadIdentity();
	_needToDraw = true;
	_popupNeedToDraw = true;

    
}

axSize axCore::GetGlobalSize() const
{
	return _size;
}

void axCore::SetGlobalSize(const axSize& size)
{
    _size = size;
}

void axCore::UpdateAll()
{
	_needToDraw = true;
	_popupNeedToDraw = true;
}

int axCore::DrawGLScene()
{
	if (_needToDraw == true)
	{
		_needToDraw = false;
	
		// Clear screen and depth buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set projection matrix.
        //axMatrix4 proj;
        //axOrtho2D(proj.Identity().GetData(), _size);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, _size.x, _size.y, 0);
		//gl
        
        // Set modelview matrix.
		//glMatrixMode(GL_MODELVIEW);
  //      axMatrix4 mview;
  //      glLoadMatrixd(mview.Identity().GetData());
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

        // Draw black rectangle.
        glColor4d(0.0, 0.0, 0.0, 1.0);
        
		axFloatRect rect(-1.0, -1.0, _size.x * 2.0, _size.y * 2.0);
        axRectFloatPoints points = rect.GetPoints();
        GLubyte indices[] = {0, 1,2, 2,3};
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_DOUBLE, 0, &points);
        glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE, indices);
        glDisableClientState(GL_VERTEX_ARRAY);

        // Draw all windows.
		_windowManager->OnPaint();
        _popupManager->OnPaint();
        
		return true;
	}
	return false;
}

int axCore::DrawGLPopScene()
{
	//glViewport(0, 0, att.width, att.height);

	//if (_needToDraw)
	if (_popupNeedToDraw)
	{
		_popupNeedToDraw = false;

		// Clear screen and depth buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glViewport(0, 0, _size.x, _size.y);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, _popSize.x, _popSize.y, 0.0);
        
        
        
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		axFloatRect rect(0 - 1.0, 0 - 1, _size.x * 2.0, _size.y * 2.0);

		glColor3f(0.1, 0.1, 0.1);
		GLfloat z = 0;

		glBegin(GL_QUADS);
		// Bottom left.
		glVertex3f(rect.position.x, rect.position.y, z);

		// Bottom Right.
		glVertex3f(rect.position.x + rect.size.x,
			rect.position.y, z);

		// Top Right.
		glVertex3f(rect.position.x + rect.size.x,
			rect.position.y + rect.size.y, z);

		// Top Left
		glVertex3f(rect.position.x, rect.position.y + rect.size.y, z);
		glEnd();

		_popupManager->OnPaint();
		return true;
	}
	return false;
}