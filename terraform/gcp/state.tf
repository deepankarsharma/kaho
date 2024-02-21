terraform {
  backend "gcs" {
    bucket  = "kaho-terraform-state"
    prefix  = "state"
  }
}