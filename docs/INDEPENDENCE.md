# Independence plan

This document records how Rouvre separates from the TextMate project and its infrastructure, and moves its own services to <https://complexuslab.fr/>. It lists what remains tied to upstream, the work in phases, and the decisions still open.

Status: plan. Nothing below is implemented unless the status table in [README.md](../README.md) says so.

## Two limits

**Legal.** The GNU General Public License, version 3, does not allow the copyright notices of the inherited code to be removed, and the git history keeps the upstream commits. Rouvre can stop depending on TextMate. It remains a derived work, and the legal notices keep saying so.

**Compatibility.** Some inherited names are a public interface: the `TM_*` environment variables, the `mate` command, the `txmt://` URL scheme, the `.tmbundle` format and the scope names. Existing bundles rely on them, and other editors read the same bundle and grammar formats. Removing them would cut Rouvre off from every existing bundle for a gain in appearance only.

The plan therefore aims at full independence of infrastructure and brand, and keeps the bundle interface working through aliases.

## Remaining ties

Inventory taken on `main` at commit `f719f92e`.

| Tie | Where | Severity |
|---|---|---|
| Bundle index fetched from `api.textmate.org` | `default.rave` (`REST_API`), `Frameworks/BundlesManager/src/BundlesManager.mm`, `Frameworks/updater/src/updater.cc` | Blocking: used at run time and at build time |
| Signing keys of the upstream team compiled in | `BundlesManager.mm`, `publicKeys` | Blocking |
| License module: DNS query to `bl.textmate.org`, link to `shop.macromates.com`, keychain item “TextMate license” | `Frameworks/license` | High: outgoing network request |
| Seven submodules, six of them under `github.com/textmate` | `.gitmodules` | High: supply chain |
| Document icons carrying the upstream brand | `Applications/TextMate/icons` submodule | Medium |
| File types exported as `com.macromates.textmate.*` | `Applications/TextMate/Info.plist` | Medium |
| Internal identifiers `com.macromates.*` | privileged helper, commit window | Medium |
| Help book “TextMate 2 Help”, JavaScript object `TextMate`, directory `Applications/TextMate` | resources and source tree | Low |
| Release script of the upstream author | `local-orig.rave` | Low |

Already done: the application name, bundle identifier, icon and data folder; software update and crash reporting switched off; GNU libiconv bundled; minimum system raised to macOS 15.

## Phases

### Phase 0 — Foundation

Merge the unit test work once it is green. Every later phase changes code those tests cover.

### Phase 1 — Cut the network ties

Goal: no request from Rouvre goes to `textmate.org` or `macromates.com`.

- Remove the license module. Rouvre is free software, registration has no purpose, and the blacklist lookup contacts a third-party server.
- Replace `REST_API` with a configurable base URL that points to complexuslab.fr.
- Remove the upstream public keys and the “Submit to MacroMates” preference.
- Add a CI check that fails when an upstream host name appears in the built application.

### Phase 2 — Hosting on complexuslab.fr

The site is served by Hostinger behind its CDN, so everything is published as static files.

| Path | Content |
|---|---|
| `/rouvre/` | Presentation and download page |
| `/rouvre/releases/` | Release manifest, archives, detached signatures |
| `/rouvre/bundles/` | Bundle index, archives, detached signatures |
| `/rouvre/manual/` | Online manual |

- **Signatures.** The current scheme signs with DSA and SHA-1 through a deprecated system API and carries the signature in HTTP response headers, which a CDN may alter. Replace it with Ed25519 signatures stored as detached files next to each download. The private key stays out of the repository.
- **Software update.** Point it at the release manifest on complexuslab.fr.
- **Publishing.** A GitHub Actions workflow builds, signs and uploads the files.
- **Mirror.** Publish each release on GitHub Releases as well, so that the hosting provider is not a single point of failure.
- **Crash reports.** Do not rebuild a collector. Crash reports contain paths and user names, which are personal data, and would need server-side processing. Reports go through the issue tracker.

### Phase 3 — Supply chain

- Mirror the submodules under the `complexus-lab` organisation, or point them at their original projects where those exist (Onigmo is maintained by its author).
- Replace the document icons with a set of Rouvre's own.
- Settle the source of the bundles (see the open decisions).

### Phase 4 — Names and interfaces, with a compatibility layer

- A `rouvre` command, with `mate` kept as an alias.
- A `rouvre://` URL scheme next to `txmt://`.
- File types declared under `fr.complexuslab.rouvre.*`. The `com.macromates.textmate.*` types move from exported to imported declarations, which is the correct form for types owned by someone else.
- Internal `com.macromates.*` identifiers renamed.
- A `Rouvre` JavaScript object, with `TextMate` kept as an alias for existing bundles.
- Source directory renamed to `Applications/Rouvre`.

This phase is the largest and the least urgent. It is split by interface, one pull request each.

### Phase 5 — Documentation and presence

Adapt and rename the manual, update the legal notices, remove `local-orig.rave`, publish the presentation page. The upstream release notes stay as an archive.

## Risks

1. **One dependency replaces another.** Hosting everything on one shared hosting account makes Rouvre depend on that provider. The GitHub Releases mirror limits this.
2. **Single maintainer.** No infrastructure choice changes the fact that the project depends on one person.
3. **Rights on the bundles.** Many bundle repositories carry no explicit license. Copying them to complexuslab.fr makes the project their redistributor. An index hosted and signed by the project that points to the original archives avoids this, at the price of a remaining dependency on those archives.
4. **Recurring cost of public distribution.** Without an Apple developer account and notarization, every user has to lift the quarantine by hand.

## Open decisions

| Subject | Options | Recommendation |
|---|---|---|
| Bundle interface (`TM_*`, `mate`, `txmt://`) | Keep as aliases, or remove | Keep as aliases |
| Bundles | Full mirror, selection with a clear license, or own index pointing to the original archives | Own index pointing to the original archives, then a mirror of the clearly licensed selection |
| Crash reports | Own collector, or none | None |
| Apple developer account | Subscribe, or distribute without notarization | Subscribe if public distribution is a goal |
| Upload to the hosting account | SFTP, Git deployment, or manual upload | To be decided |
| Signing key | On the maintainer's machine only, or also as a GitHub Actions secret | GitHub Actions secret, so that releases can be automated |

## Order of work

Phases 1 and 2 carry most of the value: they remove every dependency at run time. Phase 3 secures the build. Phase 1 depends on none of the open decisions. Phase 2 needs the upload method and the signing key decision.
