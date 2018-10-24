#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <algorithm>

// Official OpenCV
#include "opencv2\\core\\core.hpp"  
#include "opencv2\\highgui\\highgui.hpp" 
#include "opencv2\\imgproc\\imgproc.hpp"
#include "opencv2\\video\\tracking_c.h"
#include "opencv2\\calib3d.hpp"

#ifdef _DEBUG
#pragma comment(lib, "opencv_world341d.lib")
#else
#pragma comment(lib, "opencv_world341.lib")
#endif



