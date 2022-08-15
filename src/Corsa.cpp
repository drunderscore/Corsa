/*
 * Copyright (c) 2022, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Corsa.h"

namespace Corsa
{
std::map<SteamAPICall_t, std::function<void(void*)>> s_callbacks_for_async_calls;
std::map<int, std::function<void(void*)>> s_callbacks_for_callbacks;

void initialize() { SteamAPI_ManualDispatch_Init(); }

void update()
{
    SteamAPI_ManualDispatch_RunFrame(SteamAPI_GetHSteamPipe());

    CallbackMsg_t callback_message{};
    while (SteamAPI_ManualDispatch_GetNextCallback(SteamAPI_GetHSteamPipe(), &callback_message))
    {
        if (callback_message.m_iCallback == SteamAPICallCompleted_t::k_iCallback)
        {
            auto& steam_api_call_completed = reinterpret_cast<SteamAPICallCompleted_t&>(*callback_message.m_pubParam);

            auto maybe_callback_iterator = s_callbacks_for_async_calls.find(steam_api_call_completed.m_hAsyncCall);
            if (maybe_callback_iterator == s_callbacks_for_async_calls.end())
                continue; // We don't have a callback for this async call, so just ignore it.

            char callback_data_bytes[steam_api_call_completed.m_cubParam];
            bool failed;
            if (SteamUtils()->GetAPICallResult(steam_api_call_completed.m_hAsyncCall, &callback_data_bytes,
                                               sizeof(callback_data_bytes), steam_api_call_completed.m_iCallback,
                                               &failed) &&
                !failed)
            {
                maybe_callback_iterator->second(callback_data_bytes);
            }
            else
            {
                // FIXME: Would be nice to propagate the failure reason through here.
                //        I also don't like the thought of null representing failure...
                maybe_callback_iterator->second(nullptr);
            }

            s_callbacks_for_async_calls.erase(steam_api_call_completed.m_hAsyncCall);
        }
        else if (auto callback_iterator = s_callbacks_for_callbacks.find(callback_message.m_iCallback);
                 callback_iterator != s_callbacks_for_callbacks.end())
        {
            callback_iterator->second(callback_message.m_pubParam);
        }
        SteamAPI_ManualDispatch_FreeLastCallback(SteamAPI_GetHSteamPipe());
    }
}

void remove_listener(int callback_id) { s_callbacks_for_callbacks.erase(callback_id); }
}
