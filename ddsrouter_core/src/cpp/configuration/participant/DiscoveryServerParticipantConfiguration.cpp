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
 * @file DiscoveryServerParticipantConfiguration.cpp
 */

#include <ddsrouter_core/configuration/participant/DiscoveryServerParticipantConfiguration.hpp>
#include <cpp_utils/exception/ConfigurationException.hpp>
#include <ddsrouter_core/types/security/tls/TlsConfiguration.hpp>
#include <cpp_utils/Log.hpp>

namespace eprosima {
namespace ddsrouter {
namespace core {
namespace configuration {

using namespace eprosima::ddsrouter::core::types;

DiscoveryServerParticipantConfiguration::DiscoveryServerParticipantConfiguration(
        const ParticipantId& id,
        const ParticipantKind& kind,
        const bool is_repeater,
        const DomainId& domain_id,
        const GuidPrefix& discovery_server_guid_prefix,
        const std::set<Address>& listening_addresses,
        const std::set<DiscoveryServerConnectionAddress>& connection_addresses,
        const types::security::TlsConfiguration& tls_configuration)
    : SimpleParticipantConfiguration(id, kind, is_repeater, domain_id)
    , discovery_server_guid_prefix(discovery_server_guid_prefix)
    , listening_addresses(listening_addresses)
    , connection_addresses(connection_addresses)
    , tls_configuration(tls_configuration)
{
    // Do nothing
}

bool DiscoveryServerParticipantConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    // Check parent class validity
    if (!SimpleParticipantConfiguration::is_valid(error_msg))
    {
        return false;
    }

    // Check DS Guid Prefix
    if (!discovery_server_guid_prefix.is_valid())
    {
        error_msg << "Non valid Participant Guid Prefix " << discovery_server_guid_prefix << ". ";
        return false;
    }

    // Check listening addresses
    for (Address address : listening_addresses)
    {
        if (!address.is_valid())
        {
            error_msg << "Incorrect address " << address << " in listening addresses. ";
            return false;
        }
    }

    // Check connection addresses
    for (DiscoveryServerConnectionAddress address : connection_addresses)
    {
        if (!address.is_valid())
        {
            error_msg << "Incorrect address " << address << " in connection addresses. ";
            return false;
        }
    }

    // Check exist at least one address
    if (listening_addresses.empty() && connection_addresses.empty())
    {
        error_msg << "No listening or connection address specified. ";
        return false;
    }

    // If active, check it is valid
    if (tls_configuration.is_active())
    {
        // If has listening addresses, it should be able to provide TLS server configuration
        if (!listening_addresses.empty())
        {
            if (!tls_configuration.compatible<types::security::TlsKind::server>())
            {
                error_msg << "TLS requires to support Server Configuration if listening addresses set. ";
                return false;
            }
        }

        // If has connection addresses, it should be able to provide TLS client configuration
        if (!connection_addresses.empty())
        {
            if (!tls_configuration.compatible<types::security::TlsKind::client>())
            {
                error_msg << "TLS requires to support Client Configuration if connection addresses set. ";
                return false;
            }
        }
    }

    return true;
}

bool DiscoveryServerParticipantConfiguration::operator ==(
        const DiscoveryServerParticipantConfiguration& other) const noexcept
{
    // TODO
    return false;
}

} /* namespace configuration */
} /* namespace core */
} /* namespace ddsrouter */
} /* namespace eprosima */
