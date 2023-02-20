/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2021 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/
#include "res_embed.h"

#include <algorithm>
#include <map>
#include <miopen/kernel.hpp>
#include <miopen/stringutils.hpp>

namespace miopen {

const std::map<std::string, std::string>& kernel_includes()
{
    static std::map<std::string, std::string> data;
    
    if (data.size()) return data;

    // Get all entries from the index of embedded resources.
    std::vector<std::tuple<std::string, const char*, size_t, std::string> >
	    resources = res::embed::get_all();
    for (auto& resource : resources)
    {
	    auto& key = std::get<0>(resource);
	    auto& ptr = std::get<1>(resource);
            auto& size = std::get<2>(resource);

	    // Should not happen, but who knows?
	    if (!ptr) continue;

            // Consider only resource, which key looks like a header file name.
            if (!EndsWith(key, "_hpp") && !EndsWith(key, "_h"))
		    continue;

            // Replace the last "_" with "." in the key.
            auto dot = key.find_last_of('_');
            if (dot != std::string::npos)
                key[dot] = '.';

	    data.emplace(key, std::string(ptr, size));
    }

    return data;
}

std::string GetKernelInc(std::string key)
{
    auto it = kernel_includes().find(key);
    if(it == kernel_includes().end())
        MIOPEN_THROW("Failed to load kernel source: " + key);
 
    return it->second;
}

/// \todo [Optimization] Kernels are stored as strings in the static constant object.
/// Let's eliminate unnecessary copying: return pointer to the string.
/// Get rid of GetKernelInc().

const std::string* GetKernelIncPtr(std::string key)
{
    auto it = kernel_includes().find(key);
    if(it == kernel_includes().end())
        MIOPEN_THROW("Failed to load kernel source: " + key);

    return &(it->second);
}

/// \todo [Optimization] Kernel storage is constant.
/// Collect lists in the static object during the first
/// invocation and return that object on subsequent calls.

std::vector<std::string> GetKernelIncList()
{
    std::vector<std::string> keys;
    auto m = kernel_includes();
    std::transform(m.begin(),
                   m.end(),
                   std::back_inserter(keys),
                   [](decltype(m)::value_type const& pair) { return pair.first; });
    return keys;
}

std::vector<std::string> GetHipKernelIncList()
{
    auto keys = GetKernelIncList();
    keys.erase(std::remove_if(keys.begin(),
                              keys.end(),
                              [&](const auto& key) {
                                  return !(EndsWith(key, ".hpp") || EndsWith(key, ".h"));
                              }),
               keys.end());
    return keys;
}

} // namespace miopen
