/*
 * Copyright (c) 2022, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <functional>
#include <map>
#include <steam/steam_api.h>

namespace Corsa
{
extern std::map<SteamAPICall_t, std::function<void(void*)>> s_callbacks_for_async_calls;
extern std::map<int, std::function<void(void*)>> s_callbacks_for_callbacks;

void initialize();
void update();

template<typename TCallbackType>
void await(SteamAPICall_t steam_api_call, std::function<void(TCallbackType*)>&& callback)
{
    s_callbacks_for_async_calls[steam_api_call] = std::move(*reinterpret_cast<std::function<void(void*)>*>(&callback));
}

// TODO: Might be nice if this had a requires clause for TCallbackType having k_iCallback
// FIXME: Should we allow multiple listeners? Is that an expectation of an API that looks like this?
template<typename TCallbackType>
void listen(std::function<void(TCallbackType*)>&& callback)
{
    s_callbacks_for_callbacks[TCallbackType::k_iCallback] =
        std::move(*reinterpret_cast<std::function<void(void*)>*>(&callback));
}

void remove_listener(int callback_id);

template<typename TCallbackType>
void remove_listener()
{
    remove_listener(TCallbackType::k_iCallback);
}
}