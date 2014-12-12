/// @defgroup Core 
/// @{

#ifndef __AX_MANAGER__
#define __AX_MANAGER__

#include <map>

//#include "axID.h"
#include "axC++.h"
#include "axWindow.h"
#include "axWindowTree.h"
//#include "axEventManager.h"

typedef std::map<axID, axWindow*> axWindowMap;
typedef std::pair<axID, axWindow*> axWindowPair;

class axManager
{
public:
	axManager();
	~axManager();

	void InitManager(const axSize& size);

	void Add(axWindow* win);
    void AddPriorityWindow(axWindow* win);

	bool IsGrab();
	bool IsMouseHoverWindow(axWindow* win);

	// Events.
	void OnPaint();
	void OnFocusIn();
	void OnUpdate();
	void OnSize();

	// Mouse events.
	void OnMouseMotion(const axPoint& pos);
    void OnMouseLeftDragging(const axPoint& pos);
	void OnMouseLeftDown(const axPoint& pos);
    void OnMouseLeftDoubleClick(const axPoint& pos);
	void OnMouseLeftUp(const axPoint& pos);
	void OnMouseRightDown();
	void OnMouseRightUp();
	void GrabMouse(axWindow* win);
	void UnGrabMouse();

    // Keyboard events.
    // void OnCtrl();
    // void OnAlt();
    // void OnShift();
    void OnKeyDown(const char& key);
    void OnKeyUp(const char& key);
    void OnKeyDeleteDown();
    void OnBackSpaceDown();
    void OnLeftArrowDown();
    void OnRightArrowDown();
    
    void GrabKey(axWindow* win);
    void UnGrabKey();
    
    bool IsKeyGrab(axWindow* win) const;

    // bool IsCtrlDown();
    // bool IsAltDown();
    // bool IsShiftDown();
    
    bool IsEventReachWindow() const;

    std::string _managerName;

private:
	axWindowMap _windows; // Map for drawing windows.
	axWindowTree _windowTree;

	// Mouse.
	axWindow* _mouseCaptureWindow;
	axWindow* _pastWindow;
	axWindow* _currentWindow;
	axPoint _mousePosition;
    
    axWindow* _keyGrabbedWindow;
    
    bool _evtHasReachWindow;
    
    
    
    void VerifyAndProcessWindowChange();

};

#endif //__AX_MANAGER__

/// @}
