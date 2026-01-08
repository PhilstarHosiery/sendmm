SENDMATTERMOST
================================================================================

A lightweight, native C++ CLI utility for Philstar Hosiery Inc. that pipes 
standard input (stdin) directly to a specific Mattermost channel. This is 
designed to replace the legacy Java "SendMatrix" tool.

It is ideal for server automation, cron jobs, and backup scripts.
(e.g., backup.sh | SendMattermost ...)


PREREQUISITES (FreeBSD)
--------------------------------------------------------------------------------
Before compiling, ensure the required dependencies are installed on the 
FreeBSD server:

    pkg install curl nlohmann-json cmake


INSTALLATION
--------------------------------------------------------------------------------
The project is configured to install automatically to:
/storage/philstar/biz/SendMattermost

1. Clone or Copy source files to a folder.
2. Run the following commands to build and install:

    mkdir build
    cd build
    cmake ..
    make
    make install

The executable will be placed at:
/storage/philstar/bin/SendMattermost


CONFIGURATION
--------------------------------------------------------------------------------
Create a configuration file (e.g., config.properties) with the following format. 
Do not use quotes around values.

    mattermost.server=https://mm.philstar.biz
    mattermost.username=your_bot_account
    mattermost.password=your_secure_password

Security Note: Ensure this file is readable ONLY by the user running the 
script (e.g., chmod 600 config.properties).


USAGE
--------------------------------------------------------------------------------
The program reads the message body from Standard Input (stdin).

Syntax:
    /path/to/SendMattermost <config_file_path> <channel_id>

Example 1: Simple Message
    echo "Warning: Server Update Starting..." | \
    /storage/philstar/biz/SendMattermost/bin/SendMattermost config.properties <CHANNEL_ID>

Example 2: Piping a Log File
    cat /var/log/backup.log | \
    /storage/philstar/biz/SendMattermost/bin/SendMattermost config.properties <CHANNEL_ID>


FINDING THE CHANNEL ID
--------------------------------------------------------------------------------
You cannot use the channel name (e.g., "town-square"). You must use the 
Channel ID (a 26-character alphanumeric string).

How to find it:
1. Open Mattermost in a web browser.
2. Go to the channel you want to send messages to.
3. Click the Channel Name (header) > View Info.
4. Copy the "ID" string at the bottom (e.g., pg4s...).


TROUBLESHOOTING
--------------------------------------------------------------------------------
* Login Failed: 
  Check your username/password in the config file. Ensure the user is not 
  deactivated.

* Mattermost rejected post (HTTP 403): 
  The user logged in successfully but does not have permission to post in 
  that specific channel. Check that the bot account has joined the channel 
  matching the ID.

* nlohmann/json header not found: 
  Run "pkg install nlohmann-json".
