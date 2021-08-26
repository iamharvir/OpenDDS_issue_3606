#include "GuidPartitionTable.h"
namespace RtpsRelay {

GuidPartitionTable::Result GuidPartitionTable::insert(const OpenDDS::DCPS::GUID_t& guid, const DDS::StringSeq& partitions)
{
  Result result;
  std::vector<RelayPartitions> relay_partitions;
  SpdpReplay spdp_replay;

  {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, mutex_, NO_CHANGE);

    StringSet parts;
    for (CORBA::ULong idx = 0; idx != partitions.length(); ++idx) {
      parts.insert(partitions[idx].in());
    }
    if (parts.empty()) {
      // Special case for empty list of partitions.
      parts.insert("");
    }

    const auto r = guid_to_partitions_.insert(std::make_pair(guid, StringSet()));
    result = r.second ? ADDED : UPDATED;
    auto& x = r.first->second;

    std::vector<std::string> to_add;
    std::set_difference(parts.begin(), parts.end(), x.begin(), x.end(), std::back_inserter(to_add));

    std::vector<std::string> to_remove;
    std::set_difference(x.begin(), x.end(), parts.begin(), parts.end(), std::back_inserter(to_remove));

    if (to_add.empty() && to_remove.empty()) {
      // No change.
      return NO_CHANGE;
    }

    remove_from_cache(guid);

    populate_replay(spdp_replay, guid, to_add);

    StringSet globally_new;
    {
      x.insert(to_add.begin(), to_add.end());
      for (const auto& part : to_add) {
        const auto q = partition_to_guid_.insert(std::make_pair(part, OrderedGuidSet()));
        q.first->second.insert(guid);
        partition_index_.insert(part, guid);
        if (q.second) {
          globally_new.insert(part);
        }
      }
      for (const auto& part : to_remove) {
        x.erase(part);
        partition_to_guid_[part].erase(guid);
        partition_index_.remove(part, guid);
        if (partition_to_guid_[part].empty()) {
          partition_to_guid_.erase(part);
        }
      }
      if (x.empty()) {
        guid_to_partitions_.erase(r.first);
      }
    }

    add_new(relay_partitions, globally_new);
  }

  {
    ACE_GUARD_RETURN(ACE_Thread_Mutex, g, write_mutex_, NO_CHANGE);
    write_relay_partitions(relay_partitions);

    if (!spdp_replay.partitions().empty() && spdp_replay_writer_->write(spdp_replay, DDS::HANDLE_NIL) != DDS::RETCODE_OK) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) ERROR: failed to write Relay Partitions\n")));
    }
  }

  return result;
}

}
