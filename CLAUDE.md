# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SendMattermost (sendmm) is a lightweight C++17 CLI utility that pipes stdin to Mattermost channels via webhooks. Designed for server automation, cron jobs, and backup scripts on FreeBSD.

## Build Commands

```bash
mkdir build && cd build
cmake ..
make
make install   # installs to /usr/local/bin/sendmm
```

Prerequisites on FreeBSD: `pkg install curl nlohmann-json cmake`

## Architecture

Single-file application (`src/main.cpp`) with three components:
- **Config loader** (`loadWebhookUrl`): Parses key=value config file for `mattermost.webhook_url`
- **Webhook sender** (`sendWebhook`): POSTs JSON payload via libcurl with optional channel override
- **Main controller**: Reads stdin, orchestrates config loading and message dispatch

## Configuration

Config files use simple `key=value` format without quotes:
```
mattermost.webhook_url=https://mattermost.url/hooks/hook_key
```

Usage: `echo "message" | sendmm <config_file> [channel]`

Channel override must be the URL slug (e.g., `town-square`), not display name or ID.

## Dependencies

- libcurl (HTTP client)
- nlohmann/json (JSON serialization, header-only)
- CMake 3.10+ (build system)
