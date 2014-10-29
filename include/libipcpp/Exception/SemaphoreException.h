/**  This file is part of libipc++
 * 
 *  Copyright (C) 2014 leonhardt schwarz <if12b076@technikum-wien.at>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef SEMAPHORE_EXCEPTION
#define SEMAPHORE_EXCEPTION

#include "IpcException.h"
#include <string>
#include <exception>

namespace ipc
{
    class LIBIPC_API SemaphoreException : public IpcException
    {
    SemaphoreException ( const std::string& );
    SemaphoreException ( const std::string , const exception& ex );
    virtual const char* what() const;
    virtual ~SemaphoreException();
    };

}


#endif //SEMAPHORE_EXCEPTION