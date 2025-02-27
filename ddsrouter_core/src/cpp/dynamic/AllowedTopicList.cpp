// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file AllowedTopicList.cpp
 *
 */

#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/utils.hpp>

#include <dynamic/AllowedTopicList.hpp>

namespace eprosima {
namespace ddsrouter {
namespace core {

using namespace eprosima::ddsrouter::core::types;

// TODO: Add logs
AllowedTopicList::AllowedTopicList(
        const std::set<std::shared_ptr<DdsFilterTopic>>& allowlist,
        const std::set<std::shared_ptr<DdsFilterTopic>>& blocklist) noexcept
{
    allowlist_ = AllowedTopicList::get_topic_list_without_repetition_(allowlist);
    blocklist_ = AllowedTopicList::get_topic_list_without_repetition_(blocklist);

    logDebug(DDSROUTER_ALLOWEDTOPICLIST, "New Allowed topic list created:");
    logDebug(DDSROUTER_ALLOWEDTOPICLIST, "New Allowed topic list created: " << *this << ".");
}

AllowedTopicList& AllowedTopicList::operator =(
        const AllowedTopicList& other)
{
    this->allowlist_ = other.allowlist_;
    this->blocklist_ = other.blocklist_;

    return *this;
}

AllowedTopicList::~AllowedTopicList()
{
    // Eliminate all topics
    clear();
}

void AllowedTopicList::clear() noexcept
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    blocklist_.clear();
    allowlist_.clear();
}

bool AllowedTopicList::is_topic_allowed(
        const DdsTopic& topic) const noexcept
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    // It is accepted by default if allowlist is empty, if not it should pass the allowlist filter
    bool accepted = allowlist_.empty();

    // Check if allowlist filter it (this will do anything if empty and accepted will be true)
    for (std::shared_ptr<DdsFilterTopic> filter : allowlist_)
    {
        if (filter->matches(topic))
        {
            accepted = true;
            break;
        }
    }

    // Check if it has not passed the allowlist so blocklist is skipped
    if (!accepted)
    {
        return false;
    }

    // Allowlist passed, check blocklist
    for (std::shared_ptr<DdsFilterTopic> filter : blocklist_)
    {
        if (filter->matches(topic))
        {
            return false;
        }
    }

    // Blocklist passed, the topic is allowed
    return true;
}

bool AllowedTopicList::is_service_allowed(
        const RPCTopic& topic) const noexcept
{
    // Lock mutex to perform the verification atomically
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return is_topic_allowed(topic.request_topic()) && is_topic_allowed(topic.reply_topic());
}

bool AllowedTopicList::operator ==(
        const AllowedTopicList& other) const noexcept
{
    return
        utils::are_set_of_ptr_equal<DdsFilterTopic>(allowlist_, other.allowlist_) &&
        utils::are_set_of_ptr_equal<DdsFilterTopic>(blocklist_, other.blocklist_);
}

std::set<std::shared_ptr<DdsFilterTopic>> AllowedTopicList::get_topic_list_without_repetition_(
        const std::set<std::shared_ptr<DdsFilterTopic>>& list) noexcept
{
    std::set<std::shared_ptr<DdsFilterTopic>> non_repeated_list;

    // Store each topic without repetition
    for (std::shared_ptr<DdsFilterTopic> new_topic : list)
    {
        bool add_it = true;
        std::set<std::shared_ptr<DdsFilterTopic>> repeated;

        // Check if it is contained or contains any topic already in the list
        for (std::shared_ptr<DdsFilterTopic> topic_stored : non_repeated_list)
        {
            if (topic_stored->contains(*new_topic))
            {
                // It is repeated, so it must not be added
                add_it = false;
                break;
            }
            else if (new_topic->contains(*topic_stored))
            {
                // There is a repeated topic stored, pop it and add this one instead
                repeated.insert(topic_stored);
            }
        }

        // Remove topics repeated
        for (std::shared_ptr<DdsFilterTopic> topic_repeated : repeated)
        {
            non_repeated_list.erase(topic_repeated);
        }

        // Add new topic if it is not repeated
        if (add_it)
        {
            non_repeated_list.insert(new_topic);
        }
    }

    return non_repeated_list;
}

std::ostream& operator <<(
        std::ostream& os,
        const AllowedTopicList& atl)
{
    os << "AllowedTopicList{";

    // Allowed topics
    os << "allowed";
    utils::container_to_stream<std::shared_ptr<DdsFilterTopic>, true>(os, atl.allowlist_);

    // Blocked topics
    os << "blocked";
    utils::container_to_stream<std::shared_ptr<DdsFilterTopic>, true>(os, atl.blocklist_);

    os << "}";

    return os;
}

} /* namespace core */
} /* namespace ddsrouter */
} /* namespace eprosima */
