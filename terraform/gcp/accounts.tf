locals {
buildkite_permissions = [
    "roles/secretmanager.secretAccessor",
    "roles/iam.serviceAccountUser",
    "roles/iam.serviceAccountTokenCreator",
  ]
}


// Create a service account called "buildkite" that can access secrets from google cloud secrets manager
resource "google_service_account" "buildkite" {
  account_id   = "buildkite"
  display_name = "Buildkite Service Account"
}

// Bind permissions to buildkite service account
resource "google_project_iam_member" "buildkite" {
  for_each = toset(local.buildkite_permissions)
  member   = "serviceAccount:${google_service_account.buildkite.email}"
  project  = data.google_project.current.project_id
  role     = each.key
}