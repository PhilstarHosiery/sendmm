# sendmm

A lightweight, native C++ CLI utility that pipes standard input (stdin)
directly to a specific Mattermost channel.

Ideal for server automation, cron jobs, and backup scripts.

```bash
backup.sh | sendmm config.conf
```

## Prerequisites (FreeBSD)

```bash
pkg install curl nlohmann-json cmake
```

## Installation

```bash
mkdir build
cd build
cmake ..
make
make install
```

The executable will be installed to `/usr/local/bin/sendmm`

## Configuration

Create a configuration file with the webhook URL. Do not use quotes around values.

```
mattermost.webhook_url=https://your-mattermost-server.com/hooks/your-hook-key
```

To create a webhook in Mattermost:
1. Go to Integrations > Incoming Webhooks > Add Incoming Webhook
2. Select a default channel and save
3. Copy the webhook URL

**Security Note:** Ensure this file is readable only by the user running the script:
```bash
chmod 600 config.conf
```

## Usage

```
sendmm <config_file> [channel]
```

**Simple message:**
```bash
echo "Backup completed successfully" | sendmm ~/.config/sendmm.conf
```

**Override channel** (use URL slug, not display name):
```bash
echo "Alert!" | sendmm ~/.config/sendmm.conf town-square
```

**Pipe a log file:**
```bash
cat /var/log/backup.log | sendmm ~/.config/sendmm.conf
```

## Channel Override

The webhook posts to its default channel unless you specify an override.
Use the channel name (URL slug), e.g., `town-square` or `climate-monitor-alerts`.

**Note:** Channel IDs and display names do not work - only the URL slug.
The webhook must have permission to post to the target channel.

## Troubleshooting

| Error | Cause |
|-------|-------|
| HTTP 400/403 | Webhook URL is invalid or lacks permission to post to the channel |
| HTTP 404 | Webhook has been deleted or URL is incorrect |
| nlohmann/json header not found | Run `pkg install nlohmann-json` |

## License

BSD 2-Clause. See [LICENSE](LICENSE).
