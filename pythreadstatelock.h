#ifndef PYTHREADSTATELOCK_H
#define PYTHREADSTATELOCK_H

#define Py_NO_ENABLE_SHARED
#include "Python.h"

class PyThreadStateLock
{
public:
    PyThreadStateLock(void)
    {
        state = PyGILState_Ensure( );
    }

    ~PyThreadStateLock(void)
    {
        PyGILState_Release( state );
    }
private:
    PyGILState_STATE state;

};

#endif // PYTHREADSTATELOCK_H
