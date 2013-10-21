/*
 * CService.h
 *
 *  Created on: 2013-4-25
 *      Author: rogeryang
 */

#ifndef CSERVICE_H_
#define CSERVICE_H_
#include "vrpc/base/CObject.h"

namespace vrpc
{
namespace base
{

class CService : public CObject
{
public:
    CService(){}
    virtual ~CService(){}
    virtual int InitWithConf( const char* pchConfFile ) = 0;
};

}
}

#endif /* CSERVICE_H_ */
