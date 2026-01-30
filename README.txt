SendMattermost
================================================================================

A lightweight, native C++ CLI utility that pipes standard input (stdin)
directly to a specific Mattermost channel.

It is ideal for server automation, cron jobs, and backup scripts.
(e.g., backup.sh | sendmm config.conf)


PREREQUISITES (FreeBSD)
--------------------------------------------------------------------------------
Before compiling, ensure the required dependencies are installed on the 
FreeBSD server:

    pkg install curl nlohmann-json cmake


INSTALLATION
--------------------------------------------------------------------------------
1. Clone or copy source files to a folder.
2. Run the following commands to build and install:

    mkdir build
    cd build
    cmake ..
    make
    make install

The executable will be installed to /usr/local/bin/sendmm


CONFIGURATION
--------------------------------------------------------------------------------
Create a configuration file with the webhook URL. Do not use quotes around values.

    mattermost.webhook_url=https://your-mattermost-server.com/hooks/your-hook-key

To create a webhook in Mattermost:
1. Go to Integrations > Incoming Webhooks > Add Incoming Webhook
2. Select a default channel and save
3. Copy the webhook URL

Security Note: Ensure this file is readable ONLY by the user running the
script (e.g., chmod 600 config.conf).


USAGE
--------------------------------------------------------------------------------
The program reads the message body from Standard Input (stdin).

Syntax:
    sendmm <config_file> [channel_name]

Example 1: Simple Message
    echo "Backup completed successfully" | sendmm ~/.config/sendmm.conf

Example 2: Override Channel (use URL slug, not display name)
    echo "Alert!" | sendmm ~/.config/sendmm.conf town-square

Example 3: Piping a Log File
    cat /var/log/backup.log | sendmm ~/.config/sendmm.conf


CHANNEL OVERRIDE
--------------------------------------------------------------------------------
The webhook posts to its default channel unless you specify an override.
Use the channel name (URL slug), e.g., "town-square" or "climate-monitor-alerts".

Note: Channel IDs and display names do not work - only the URL slug.
The webhook must have permission to post to the target channel.


TROUBLESHOOTING
--------------------------------------------------------------------------------
* HTTP 400/403:
  The webhook URL is invalid or the webhook doesn't have permission to post
  to the specified channel.

* HTTP 404:
  The webhook has been deleted or the URL is incorrect.

* nlohmann/json header not found:
  Run "pkg install nlohmann-json".
