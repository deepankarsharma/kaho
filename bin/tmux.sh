#!/bin/bash
# ["192.168.86.244"]="hw"
# ["192.168.86.27"]="mini"
# Define your host and name pairs as an associative array
declare -A hosts=(
    ["192.168.10.21"]="zen"
    ["192.168.86.129"]="m1"
    ["192.168.86.237"]="hp"
)

# Start a new tmux session named 'remote_hosts'
tmux new-session -d -s remote_hosts

# Iterate through the hosts, creating windows with SSH connections
for host in "${!hosts[@]}"; do
  tmux new-window -t remote_hosts -n "${hosts[$host]}" "ssh $host"
done

# Select the first window as the starting point
tmux select-window -t remote_hosts:0

# Attach to the session to start working
tmux attach-session -t remote_hosts
