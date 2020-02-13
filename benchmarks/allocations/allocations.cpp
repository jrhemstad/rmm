/*
 * Copyright (c) 2020, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either ex  ess or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <rmm/mr/device/cnmem_memory_resource.hpp>
#include <rmm/mr/device/cuda_memory_resource.hpp>
#include <rmm/mr/device/default_memory_resource.hpp>
#include <rmm/mr/device/device_memory_resource.hpp>
#include <rmm/mr/device/managed_memory_resource.hpp>

#include <benchmark/benchmark.h>

#include <chrono>
#include <iostream>
#include <thread>

static void BM_test(benchmark::State& state) {

  rmm::mr::cuda_memory_resource mr;

  try {
    for (auto _ : state) {
      std::this_thread::sleep_for(std::chrono::duration<int>(1));
    }
  } catch (std::exception const& e) {
    std::cout << "Error: " << e.what() << "\n";
  }
}
BENCHMARK(BM_test)->Unit(benchmark::kMillisecond);