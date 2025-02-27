// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>
#include <test_utils.hpp>

#include <ddsrouter_core/types/address/DiscoveryServerConnectionAddress.hpp>
#include <ddsrouter_yaml/YamlReader.hpp>
#include <ddsrouter_yaml/yaml_configuration_tags.hpp>

#include "../../../YamlConfigurationTestUtils.hpp"

using namespace eprosima::ddsrouter;
using namespace eprosima::ddsrouter::yaml;

const constexpr unsigned int TEST_ADDRESSES_NUMBER = 5;

/**
 * Test read a discovery server
 *
 * POSITIVE CASES:
 * - one address
 * - several addresses
 */
TEST(YamlGetEntityDiscoveryServerAddressTest, get_ds_address)
{
    // one address
    {
        Yaml yml_ds_address;

        // Get random guid prefix and add it to yaml
        core::types::GuidPrefix guid_prefix = eprosima::ddsrouter::test::random_guid_prefix();
        Yaml yml_guid;
        yaml::test::guid_prefix_to_yaml(yml_guid, guid_prefix);

        yml_ds_address[DISCOVERY_SERVER_GUID_PREFIX_TAG] = yml_guid;

        // Get random address and add it to yaml
        core::types::Address address = eprosima::ddsrouter::test::random_address();
        Yaml yml_addresses;
        Yaml yml_address;
        yaml::test::address_to_yaml(yml_address, address);

        yml_addresses.push_back(yml_address);

        yml_ds_address[COLLECTION_ADDRESSES_TAG] = yml_addresses;

        // Generate overall yaml
        Yaml yml;
        yml["ds-address"] = yml_ds_address;

        // Create object DiscoveryServerAddress from yaml
        core::types::DiscoveryServerConnectionAddress result =
                YamlReader::get<core::types::DiscoveryServerConnectionAddress>(yml, "ds-address", LATEST);

        // Check result
        ASSERT_EQ(guid_prefix, result.discovery_server_guid_prefix());
        ASSERT_EQ(result.addresses().size(), 1u);
        ASSERT_EQ(address, *result.addresses().begin());
    }

    // several addresses
    {
        Yaml yml_ds_address;

        // Get random guid prefix and add it to yaml
        core::types::GuidPrefix guid_prefix = eprosima::ddsrouter::test::random_guid_prefix();
        Yaml yml_guid;
        yaml::test::guid_prefix_to_yaml(yml_guid, guid_prefix);

        yml_ds_address[DISCOVERY_SERVER_GUID_PREFIX_TAG] = yml_guid;

        // Get random address and add it to yaml
        Yaml yml_addresses;
        std::vector<core::types::Address> addresses;
        for (unsigned int i = 0; i < TEST_ADDRESSES_NUMBER; i++)
        {
            // Create new address and add it to already created addresses and to yaml
            Yaml yml_address;
            core::types::Address address = eprosima::ddsrouter::test::random_address(i);

            yaml::test::address_to_yaml(yml_address, address);

            addresses.push_back(address);
            yml_addresses.push_back(yml_address);
        }
        yml_ds_address[COLLECTION_ADDRESSES_TAG] = yml_addresses;

        // Generate overall yaml
        Yaml yml;
        yml["ds-address"] = yml_ds_address;

        // Create object DiscoveryServerAddress from yaml
        core::types::DiscoveryServerConnectionAddress result =
                YamlReader::get<core::types::DiscoveryServerConnectionAddress>(yml, "ds-address", LATEST);

        // Check result
        ASSERT_EQ(guid_prefix, result.discovery_server_guid_prefix());
        ASSERT_EQ(result.addresses().size(), TEST_ADDRESSES_NUMBER);

        // Check every address introduced in yaml is in result
        for (core::types::Address address : addresses)
        {
            // ATTENTION: this previous declaration is needed as listening_addresses() does not return a reference
            std::set<eprosima::ddsrouter::core::types::Address> addresses = result.addresses();
            ASSERT_NE(addresses.find(address), addresses.end());
        }
    }
}
