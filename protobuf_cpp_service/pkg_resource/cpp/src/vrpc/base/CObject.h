/*
 * CObject.h
 *
 *  Created on: 2013-3-8
 *      Author: rogeryang
 */

#ifndef COBJECT_H_
#define COBJECT_H_
#include <stdarg.h>
#include <stdio.h>

namespace vrpc
{
namespace base
{

class CObject
{
public:
    CObject();
    virtual
    ~CObject();

    inline const char* GetErrorMessage()
    {
        return m_szErr;
    }

    inline void SetErrorMessage( const char* fmt, ... )
    {
        va_list args;

        va_start(args, fmt);
        vsnprintf( m_szErr, sizeof(m_szErr), fmt, args );
        va_end(args);
    }

    virtual void Reset()
    {
        m_szErr[0] = 0;
    }

private:
    char m_szErr[1024];
};

} /* namespace base */
} /* namespace vrpc */
#endif /* COBJECT_H_ */
