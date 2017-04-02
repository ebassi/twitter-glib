Twitter-GLib - A library wrapping the Twitter RESTful API
===============================================================================

**WARNING**: Twitter-GLib is unmaintained; it uses the deprecated v1 API, and
does not support the stream API. It also does not use OAuth so it's basically
useless for anything that requires authentication. Twitter-GLib is mostly meant
as a historical archive.

Twitter-GLib is a GObject-based library providing a wrapper around
the RESTful API of Twitter.

  Twitter is for staying in touch and keeping up with friends no matter
  where you are or what you’re doing. For some friends you might want
  instant mobile updates - for others, you can just check the web. Invite
  your friends to Twitter and decide how connected you want you to be.
                -- from http://twitter.com

In short, is a web-based SMS-like service.

## REQUIREMENTS

Twitter-GLib depends on:

  - GLib >= 2.16
  - GIO >= 2.16
  - JSON-GLib >= 0.6
  - libsoup-2.4 >= 2.4.1
    - or libsoup-gnome-2.4 >= 2.4.1
  - GdkPixbuf >= 2.0

## INSTALLATION

Se the INSTALL file.

## COPYRIGHT AND LICENSING

Twitter-GLib is released under the terms of the GNU Lesser General
Public License version 2.1 or, at your option, any later version.

Twitter-GLib is Copyright (C) 2008, 2009  Emmanuele Bassi. All rights reserved.
