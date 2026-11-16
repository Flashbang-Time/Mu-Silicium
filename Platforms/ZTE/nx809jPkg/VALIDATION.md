# Validation record

Upstream base: `fb4efb37b4ac289ce83ad94c7acb73defb87955c` (main, checked 16 November 2026).

The fork uses that upstream commit and its pinned dependency revisions, including Mu Basecore `64f478939955e45faa632c160ae2a75a5ff7ef7f` and Silicium-ACPI `8c4cdc43abf23cffdeb588621126253bb2c559da`. NX809J dependency modifications are stored in Tools/nx809j and applied by the preparation script rather than hidden in unpublished submodule commits.

Validated in an isolated Linux checkout with locally supplied, hash-verified NX809J assets:

- Source dependency patches applied successfully to the current pinned dependencies.
- ACPI compilation with iasl: no errors or warnings. The provider profile has an unknown-UUID remark for the existing Qualcomm GPIO interface; the PE polling profile removes that interface.
- RELEASE ARM64 build with the resident AMU workaround: success.
- Both provider and PE polling ACPI profiles compiled successfully.
- Upstream post-build image validation: 52 images verified in each successful build.

These are offline build checks. The earlier AMU-fix firmware reached Windows PE on the phone, but this upstream-refreshed rebuild has not been hardware-tested. Touch, USB, Windows storage, networking, GPU acceleration, audio and power management are not established by these checks. The signature-enforcement correction remains a separate PE boot configuration change.
