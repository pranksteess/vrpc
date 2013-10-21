#ifndef _VRPC_BASE_MEM_TOOLS_H_
#define _VRPC_BASE_MEM_TOOLS_H_
#include "vrpc/base/CObject.h"

namespace vrpc
{
namespace base
{

#define SAFE_FREE(p)\
    do\
    {\
        if( p != NULL )\
        {\
            ::free(p);\
        }\
        p = NULL;\
    }while(0)

#define SAFE_DELETE(p)\
    do\
    {\
        delete p;\
        p = NULL;\
    }while(0)

#define SAFE_DELETE_ARR(p)\
    do\
    {\
        delete[] p;\
        p = NULL;\
    }while(0)

template<class T>
class CScopePtrGuard : public CObject
{
public:
    CScopePtrGuard( T* p ) : m_p(p)
    {
    }

    ~CScopePtrGuard()
    {
        SAFE_DELETE(m_p);
    }

private:
    T* m_p;
};

class CScopePtrGuardC : public CObject
{
public:
    CScopePtrGuardC( char* p ) : m_p(p)
    {
    }

    ~CScopePtrGuardC()
    {
        SAFE_FREE(m_p);
    }

private:
    char* m_p;
};

}
}
#endif
