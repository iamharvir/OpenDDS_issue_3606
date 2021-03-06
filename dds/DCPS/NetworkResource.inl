/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "transport/framework/EntryExit.h"

#include <ace/CDR_Stream.h>
#include <ace/OS_NS_netdb.h>

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_INLINE
OpenDDS::DCPS::NetworkResource::NetworkResource()
  : reserved_(0)
{
  DBG_ENTRY_LVL("NetworkResource","NetworkResource",6);
}

ACE_INLINE
OpenDDS::DCPS::NetworkResource::NetworkResource(const ACE_INET_Addr& addr)
  : reserved_(0)
{
  DBG_ENTRY_LVL("NetworkResource","NetworkResource",6);

  ACE_TCHAR addr_s[MAXHOSTNAMELEN + 1];
  addr.addr_to_string(addr_s, sizeof (addr_s), 1 /* ipaddr_format */);

  this->addr_ = ACE_TEXT_ALWAYS_CHAR(addr_s);
}

ACE_INLINE
OpenDDS::DCPS::NetworkResource::NetworkResource(const String& addr)
  : reserved_(0)
{
  DBG_ENTRY_LVL("NetworkResource","NetworkResource",6);

  addr_ = addr;
}

ACE_INLINE
OpenDDS::DCPS::NetworkResource::~NetworkResource()
{
}

ACE_INLINE
void OpenDDS::DCPS::NetworkResource::to_addr(ACE_INET_Addr& addr) const
{
  DBG_ENTRY_LVL("NetworkResource","to_addr",6);
  addr = choose_single_coherent_address(addr_.c_str(), true /*prefer_loopback*/);
  VDBG_LVL((LM_DEBUG, "(%P|%t) NetworkResource::to_addr() - Resolving address for %C to be %C\n", addr_.c_str(), addr.get_host_addr()), 2);
}

ACE_INLINE
void OpenDDS::DCPS::NetworkResource::dump() const
{
  ACE_DEBUG((LM_DEBUG, "(%P|%t) NetworkResource addr: %C reserved: %d\n", addr_.c_str(), reserved_));
}

OPENDDS_END_VERSIONED_NAMESPACE_DECL
