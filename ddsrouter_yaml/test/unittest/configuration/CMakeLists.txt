# Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#####################################
# Yaml DDSRouter Configuration Test #
#####################################

set(TEST_NAME YamlReaderConfigurationTest)

set(TEST_SOURCES
        YamlReaderConfigurationTest.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/yaml_configuration_tags.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlManager.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReaderConfiguration.cpp
        ${PROJECT_SOURCE_DIR}/test/TestUtils/test_utils.cpp
    )

set(TEST_LIST
        get_ddsrouter_configuration_v1
        get_ddsrouter_configuration_v2
        get_ddsrouter_configuration_no_version
        version_negative_cases
        number_of_threads
        max_history_depth
    )

set(TEST_EXTRA_LIBRARIES
        yaml-cpp
        fastcdr
        fastrtps
        cpp_utils
        ddsrouter_core
    )

add_unittest_executable(
    "${TEST_NAME}"
    "${TEST_SOURCES}"
    "${TEST_LIST}"
    "${TEST_EXTRA_LIBRARIES}")
