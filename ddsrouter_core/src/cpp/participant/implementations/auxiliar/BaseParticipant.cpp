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
 * @file BaseParticipant.cpp
 */

#include <memory>

#include <ddsrouter_core/types/participant/ParticipantKind.hpp>
#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <participant/implementations/auxiliar/BaseParticipant.hpp>
#include <reader/implementations/auxiliar/BaseReader.hpp>
#include <writer/implementations/auxiliar/BaseWriter.hpp>

namespace eprosima {
namespace ddsrouter {
namespace core {

BaseParticipant::BaseParticipant(
        std::shared_ptr<configuration::ParticipantConfiguration> participant_configuration,
        std::shared_ptr <PayloadPool> payload_pool,
        std::shared_ptr <DiscoveryDatabase> discovery_database)
    : configuration_(participant_configuration)
    , payload_pool_(payload_pool)
    , discovery_database_(discovery_database)
{
    logDebug(DDSROUTER_TRACK, "Creating Participant " << *this << ".");
}

BaseParticipant::~BaseParticipant()
{
    logDebug(DDSROUTER_TRACK, "Destroying Participant " << *this << ".");

    if (!writers_.empty())
    {
        logDevError(DDSROUTER_BASEPARTICIPANT, "Deleting Participant " << id() << " with still alive writers");
        writers_.clear();
    }

    if (!readers_.empty())
    {
        logDevError(DDSROUTER_BASEPARTICIPANT, "Deleting Participant " << id() << " with still alive readers");
        readers_.clear();
    }

    logDebug(DDSROUTER_TRACK, "Participant " << *this << " destroyed.");
}

types::ParticipantId BaseParticipant::id() const noexcept
{
    std::lock_guard <std::recursive_mutex> lock(mutex_);

    return configuration_->id;
}

types::ParticipantKind BaseParticipant::kind() const noexcept
{
    std::lock_guard <std::recursive_mutex> lock(mutex_);

    return configuration_->kind;
}

bool BaseParticipant::is_repeater() const noexcept
{
    std::lock_guard <std::recursive_mutex> lock(mutex_);

    return configuration_->is_repeater;
}

bool BaseParticipant::is_rtps_kind() const noexcept
{
    switch (kind())
    {
        case types::ParticipantKind::simple_rtps:
        case types::ParticipantKind::local_discovery_server:
        case types::ParticipantKind::wan_discovery_server:
        case types::ParticipantKind::wan_initial_peers:
            return true;
        default:
            return false;
    }
}

std::shared_ptr<IWriter> BaseParticipant::create_writer(
        types::DdsTopic topic)
{
    std::lock_guard <std::recursive_mutex> lock(mutex_);

    if (writers_.find(topic) != writers_.end())
    {
        throw utils::InitializationException(
                  utils::Formatter() <<
                      "Error creating writer for topic " << topic << " in participant " << id() <<
                      ". Writer already exists.");
    }

    std::shared_ptr <IWriter> new_writer = create_writer_(topic);

    logInfo(DDSROUTER_BASEPARTICIPANT, "Created writer in Participant " << id() << " for topic " << topic);

    // Insertion must not fail as we already know it does not exist
    writers_.emplace(topic, new_writer);

    return new_writer;
}

std::shared_ptr<IReader> BaseParticipant::create_reader(
        types::DdsTopic topic)
{
    std::lock_guard <std::recursive_mutex> lock(mutex_);

    if (readers_.find(topic) != readers_.end())
    {
        throw utils::InitializationException(
                  utils::Formatter() <<
                      "Error creating Reader for topic " << topic << " in participant " << id() <<
                      ". Reader already exists.");
    }

    std::shared_ptr <IReader> new_reader = create_reader_(topic);

    logInfo(DDSROUTER_BASEPARTICIPANT, "Created reader in Participant " << id() << " for topic " << topic);

    // Insertion must not fail as we already know it does not exist
    readers_.emplace(topic, new_reader);

    return new_reader;
}

void BaseParticipant::delete_writer(
        std::shared_ptr <IWriter> writer) noexcept
{
    std::lock_guard <std::recursive_mutex> lock(mutex_);

    for (auto it_writer : writers_)
    {
        if (it_writer.second == writer)
        {
            writers_.erase(it_writer.first);
            delete_writer_(writer);
            return;
        }
    }
}

void BaseParticipant::delete_reader(
        std::shared_ptr <IReader> reader) noexcept
{
    std::lock_guard <std::recursive_mutex> lock(mutex_);

    for (auto it_reader : readers_)
    {
        if (it_reader.second == reader)
        {
            readers_.erase(it_reader.first);
            delete_reader_(reader);
            return;
        }
    }
}

void BaseParticipant::delete_writer_(
        std::shared_ptr <IWriter>) noexcept
{
    // It does nothing. Override this method so it has functionality.
}

void BaseParticipant::delete_reader_(
        std::shared_ptr <IReader>) noexcept
{
    // It does nothing. Override this method so it has functionality.
}

types::ParticipantId BaseParticipant::id_nts_() const noexcept
{
    return configuration_->id;
}

std::ostream& operator <<(
        std::ostream& os,
        const BaseParticipant& participant)
{
    os << "{" << participant.id() << ";" << participant.configuration_->kind << "}";
    return os;
}

} /* namespace core */
} /* namespace ddsrouter */
} /* namespace eprosima */
