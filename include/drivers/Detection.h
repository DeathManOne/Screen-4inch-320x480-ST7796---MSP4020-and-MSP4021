/*
 * include/drivers/Detection.h
 *
 * Copyright (c) 2026 DeathManOne
 * https://github.com/DeathManOne
 *
 * This file is part of the ST7796S library.
 *
 * ST7796S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ST7796S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ST7796S.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <cstring>

namespace ST7796S {
    /**
     * @brief Stores one controller-detection result.
     */
    struct DetectionLine {
        /** Human-readable label printed before the captured bytes. */
        const char* label = nullptr;

        /** Bytes returned by the controller. */
        uint8_t data[8] = {};

        /** Number of valid bytes in data. */
        uint8_t length = 0;
    };

    /**
     * @brief Stores the diagnostic lines collected while probing drivers.
     */
    struct DetectionReport {
        /** Maximum number of stored detection lines. */
        static constexpr uint8_t MAX_LINES = 16;

        /** Maximum number of bytes stored in one line. */
        static constexpr uint8_t MAX_BYTES_PER_LINE = 8;

        /** Detection lines in probe order. */
        DetectionLine lines[MAX_LINES] = {};

        /** Number of valid entries in lines. */
        uint8_t count = 0;

        /**
         * @brief Finds a mutable detection result.
         *
         * @param label Exact line label.
         * @param length Expected number of bytes.
         * @return Pointer to the captured bytes, or nullptr when absent.
         */
        uint8_t* find(const char* label, uint8_t length) {
            if (!label || length == 0)
                { return nullptr; }
            for (uint8_t i = 0; i < count; i++) {
                if (
                    lines[i].length == length &&
                    std::strcmp(lines[i].label, label) == 0
                ) {
                    return lines[i].data;
                }
            }
            return nullptr;
        }

        /**
         * @brief Finds a read-only detection result.
         *
         * @param label Exact line label.
         * @param length Expected number of bytes.
         * @return Pointer to the captured bytes, or nullptr when absent.
         */
        const uint8_t* find(const char* label, uint8_t length) const {
            if (!label || length == 0)
                { return nullptr; }
            for (uint8_t i = 0; i < count; i++) {
                if (
                    lines[i].length == length &&
                    std::strcmp(lines[i].label, label) == 0
                ) {
                    return lines[i].data;
                }
            }
            return nullptr;
        }

        /**
         * @brief Adds or reuses a detection line.
         *
         * A line with the same label and length is reused. A duplicate label
         * with another length is rejected.
         *
         * @param label Human-readable line label.
         * @param length Number of bytes to store.
         * @return Writable byte buffer, or nullptr when the request is invalid
         *         or the report capacity is exhausted.
         */
        uint8_t* add(const char* label, uint8_t length) {
            if (!label || length == 0)
                { return nullptr; }
            uint8_t* existing = find(label, length);
            if (existing)
                { return existing; }
            for (uint8_t i = 0; i < count; i++) {
                if (std::strcmp(lines[i].label, label) == 0)
                    { return nullptr; }
            }
            if (
                count >= MAX_LINES ||
                length > MAX_BYTES_PER_LINE
            ) {
                return nullptr;
            }
            DetectionLine& line = lines[count++];
            line.label = label;
            line.length = length;
            return line.data;
        }
    };
}
