/******************************************************************************
* Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include <libpc/drivers/las/Iterator.hpp>

#include <libpc/drivers/las/Reader.hpp>
#include <libpc/exceptions.hpp>
#include <libpc/Utils.hpp>
#include <libpc/Header.hpp>
#include <libpc/PointBuffer.hpp>

namespace libpc { namespace drivers { namespace las {


SequentialIterator::SequentialIterator(const LasReader& reader)
    : libpc::SequentialIterator(reader)
    , m_reader(reader)
    , m_istream(NULL)
{
    m_istream = Utils::openFile(m_reader.getFileName());
    return;
}


SequentialIterator::~SequentialIterator()
{
    Utils::closeFile(m_istream);
    return;
}


boost::uint64_t SequentialIterator::skipImpl(boost::uint64_t count)
{
    const LasReader& reader = m_reader;
    const Header& header = reader.getHeader();

    boost::uint32_t chunk = (boost::uint32_t)count; // BUG: this needs to be done in blocks if pointNum is large

    // BUG: we can move the stream a constant amount
    PointBuffer PointBuffer(header.getSchema(), chunk);
    read(PointBuffer);
    // just drop the points on the floor and return
    
    return count;
}


bool SequentialIterator::atEndImpl() const
{
    return getIndex() >= getStage().getNumPoints();
}


boost::uint32_t SequentialIterator::readImpl(PointBuffer& data)
{
    return m_reader.processBuffer(data, *m_istream);
}



RandomIterator::RandomIterator(const LasReader& reader)
    : libpc::RandomIterator(reader)
    , m_reader(reader)
    , m_istream(NULL)
{
    m_istream = Utils::openFile(m_reader.getFileName());
    return;
}


RandomIterator::~RandomIterator()
{
    Utils::closeFile(m_istream);
    return;
}


boost::uint64_t RandomIterator::seekImpl(boost::uint64_t)
{
    throw not_yet_implemented("LasReader seeking not supported yet");
}


boost::uint32_t RandomIterator::readImpl(PointBuffer& data)
{
    return m_reader.processBuffer(data, *m_istream);
}


} } } // namespaces
