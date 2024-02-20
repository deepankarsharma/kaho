#!/bin/bash

# Check for a single argument
if [ $# -ne 1 ]; then
  echo "Usage: $0 <secret_name>"
  exit 1
fi

# Retrieve the secret name provided as the argument
secret_name=$1

# Use gcloud to access the latest version of the secret and store it
secret_value=$(gcloud secrets versions access latest --secret="$secret_name")

# Check if the secret access was successful
if [ $? -ne 0 ]; then
  echo "Error: Failed to retrieve secret. Check secret name and permissions."
  exit 1
fi

# Print the secret value
echo "$secret_value"
