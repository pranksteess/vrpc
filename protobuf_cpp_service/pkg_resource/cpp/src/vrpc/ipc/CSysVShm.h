/*
 * CSysVShm.h
 *
 *  Created on: 2013-7-2
 *      Author: ylc8131
 */

#ifndef CSYSVSHM_H_
#define CSYSVSHM_H_
#include <sys/types.h>
#include <sys/ipc.h>
#include "vrpc/base/CObject.h"

namespace vrpc
{
namespace ipc
{

class CSysVShm : public vrpc::base::CObject
{
public:
    CSysVShm();
    virtual
    ~CSysVShm();

    int OpenAndAttach( key_t key, size_t size, int oflag );
    void* GetShmAddr(){ return m_pcData; }
    key_t GetKey(){ return m_key; }
    int GetId(){ return m_id; }

private:
    key_t m_key;
    int m_id;
    void* m_pcData;
};

} /* namespace ipc */
} /* namespace vrpc */
#endif /* CSYSVSHM_H_ */
