provider "google" {
  project = "kaho-413904"
  region  = "us-east4"
  zone    = "us-east4-a"
}

data "google_project" "current" {}