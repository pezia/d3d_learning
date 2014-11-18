#pragma once

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
	if(x != NULL)\
	{\
		x->Release();\
		x = NULL;\
	}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) \
	if(x != NULL)\
	{\
		delete x;\
		x = NULL;\
	}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) \
	if(x != NULL)\
	{\
		delete[] x;\
		x = NULL;\
	}
#endif
