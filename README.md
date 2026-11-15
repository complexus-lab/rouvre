# Rouvre

Rouvre is a [Complexus Lab](https://complexuslab.fr) project: an independent fork of the TextMate 2 source code, a text editor for macOS. Upstream development stopped in October 2021. This fork exists to make the code build and run again on current macOS releases and on Apple Silicon.

Rouvre is maintained at <https://github.com/complexus-lab/rouvre>. It has no affiliation with the original publisher or author, who neither endorse nor support it. Questions about this fork belong in this repository.

## Status

This is work in progress. There are no binary releases.

| Area | State |
|---|---|
| Source | Upstream commit `346b52b` (12 October 2021) plus the changes in this repository |
| Build | Compiles, links and signs on macOS 15 with Apple Silicon (arm64 only); the resulting application has not yet been launched or tested |
| Tests | Not runnable: the build generator does not yet emit rules for the `tests` directives |
| Naming | The application is `Rouvre.app` with the identifier `fr.complexuslab.Rouvre`. Names that bundles and scripts depend on are kept: the `mate` command, the `TM_*` variables, the `TextMate` JavaScript object and the `com.macromates.textmate.*` file types. Settings, bundles and session data live in `~/Library/Application Support/Rouvre`, separate from a TextMate installation |
| Online services | Software update and crash reporting are switched off. The bundle index is still fetched from the upstream server, which this fork does not operate |

## Feedback

Use the [issue tracker](https://github.com/complexus-lab/rouvre/issues) for bug reports and questions.

## Building

### Requirements

 * macOS with Xcode and its command line tools
 * [boost][]            — portable C++ source libraries
 * [Cap’n Proto][capnp] — serialization library
 * [multimarkdown][]    — marked-up plain text compiler
 * [ninja][]            — build system similar to `make`
 * [ragel][]            — state machine compiler
 * [sparsehash][]       — a cache friendly `hash_map`

All of these can be installed with either [Homebrew][] or [MacPorts][]:

```sh
# Homebrew
brew install boost capnp google-sparsehash multimarkdown ninja ragel

# MacPorts
sudo port install boost capnproto multimarkdown ninja ragel sparsehash
```

### Build

Clone with submodules, then run `./configure` followed by `ninja`:

```sh
git clone --recursive https://github.com/complexus-lab/rouvre.git
cd rouvre
./configure && ninja Rouvre/run
```

`./configure` checks that the dependencies can be found and calls `bin/rave` to generate `build.ninja`, with the default config set to `release` and the default target set to `Rouvre`.

The dependency location is detected in this order: the `PREFIX` environment variable, `brew --prefix`, `/opt/local` (MacPorts), `/usr/local`. To force a location:

```sh
PREFIX=/some/prefix ./configure
```

Cap’n Proto is linked statically when its archives (`libcapnp.a`, `libkj.a`) are present, through a `.static-libs` directory created by `./configure`, so that the application runs on machines where the library is not installed.

The detected paths are written to `local.rave`. Delete that file to run the detection again; if it exists, `./configure` leaves it untouched.

Build output goes to `~/build/rouvre` unless the `builddir` environment variable names an existing directory.

### Build targets

```sh
ninja Rouvre      # Build and sign the application
ninja Rouvre/run  # Build, sign, and (re)launch the application
```

The application is signed ad hoc (`CS_IDENTITY` is `-` in `default.rave`). Set `CS_IDENTITY` in `local.rave` to sign with your own Developer ID.

To clean everything run:

```sh
ninja -t clean
```

Or delete the build directory.

### Building from within the editor

Install the [Ninja][NinjaBundle] bundle, then press ⌘B. Set the `PATH` variable in _Preferences_ → _Variables_ or in `~/.tm_properties` so that `ninja` and the other tools are found, for example `$PATH:/opt/homebrew/bin`.

The default target, set in `.tm_properties`, is `Rouvre/run`. If the current file belongs to another application target, `TM_NINJA_TARGET` builds and runs that application instead.

### Continuous integration

Every pull request and every push to `main` is built on a GitHub-hosted `macos-15` runner. The build continues past the first failure; the complete log is attached to the run as the `build-log` artifact and the job summary counts the errors per message.

## Documentation

 * [INTERNALS.md](INTERNALS.md) describes the core data structures and the layering of the code.
 * [CONTRIBUTING.md](CONTRIBUTING.md) describes how to submit changes.
 * The user manual shipped with the application is in `Applications/TextMate/resources/TextMate Help`.

## Legal

The source is released under the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. See [COPYING](COPYING) and [LICENSE](LICENSE).

The code inherited from upstream is copyright MacroMates Ltd. and the upstream contributors; the complete authorship is preserved in the git history. Changes made in this repository are copyright Thierry Sayegh-Sauvage (Complexus Lab) and their respective authors, and are released under the same license. The license requires the inherited copyright notices to stay in place.

“TextMate” is a trademark of its owner, who has no connection with this fork. The name appears here only to state where the code comes from.

[boost]:         https://www.boost.org/
[ninja]:         https://ninja-build.org/
[multimarkdown]: https://fletcherpenney.net/multimarkdown/
[ragel]:         https://www.colm.net/open-source/ragel/
[capnp]:         https://capnproto.org/
[sparsehash]:    https://github.com/sparsehash/sparsehash
[Homebrew]:      https://brew.sh/
[MacPorts]:      https://www.macports.org/
[NinjaBundle]:   https://github.com/textmate/ninja.tmbundle
