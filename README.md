# Image Diff Tool

Diff tool for images to use in a Git based png-review workflow.

## Purpose

I use this to diff screenshots before committing them to the Git repo.
It is on Github as a backup for me and because others might be useful.

This is not a professional tool or claims to be bugfree. Use it on your own risk.

## Requirements

* Linux
* Qt6
* CMake

## Installation

You need to compile it from source.

This is how I build it:
```
mkdir bld && cd bld
cmake -GNinja  -DCMAKE_INSTALL_PREFIX=$HOME ..
cmake --build .
cmake --install .
```

Ensure ~/bin is on your PATH so that the tool can be found.

Then add this to your `~/.gitconfig`.

```
[alias]
    png-review = "!sh ~/bin/git-png-review"
```
## Usage

In any Git repo with modified .png files you can type `git png-review` and it will fire up this GUI,
which shows the diff between old and new screenshots.

[Screenshot](screenshot.png)

* Click 'Accept' to accept the change. This will call `git add <file>` for you.
* Click 'Skip' to ignore the change. It will open the next image.
* Click 'Abort' to abort the review process.
