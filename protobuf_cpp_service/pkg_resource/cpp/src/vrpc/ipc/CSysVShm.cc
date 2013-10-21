/*
 * CSysVShm.cpp
 *
 *  Created on: 2013-7-2
 *      Author: ylc8131
 */
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include "CSysVShm.h"

namespace vrpc
{
namespace ipc
{

    CSysVShm::CSysVShm() : m_key(-1), m_id(-1), m_pcData(NULL)
    {
        // TODO Auto-generated constructor stub
    }

    CSysVShm::~CSysVShm()
    {
        // TODO Auto-generated destructor stub
    }

    int CSysVShm::OpenAndAttach( key_t key, size_t size, int oflag )
    {
        int shm_id = shmget( key, size, oflag );
        if( -1 == shm_id )
        {
            SetErrorMessage( "shmget failed.err:%s", strerror(errno) );
            return -10;
        }

        void* data = shmat( shm_id, NULL, 0 );
        if( -1 == (long)data )
        {
            SetErrorMessage( "shmat failed.err:%s", strerror(errno) );
            return -20;
        }

        m_key = key;
        m_id = shm_id;
        m_pcData = data;

        return 0;
    }

} /* namespace ipc */
} /* namespace vrpc */
