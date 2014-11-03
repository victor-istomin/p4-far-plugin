TODO List
=========

Legend
------
* [*] Task in progress
* [+] Done task
* Planned task

Functionality
-------------
* P4 login when logged out
 - login menu support
  + [+] save login, worspace, password dialog
  + [*] support multiple credentials in settings
  + login dialog integration to login menu
  + authenticate using stored MD5(password)?
* refactoring
 - rename '.+Guid' to 'Guid.+'
 - rename 'FarMenu::MenuItems' to 'FarMenu::Items'
 - move menu items handler to separate class
* auto 'p4 edit' when editing versioned readonly file
* support for long messages (word wrap) in errors message box
* P4 Client informationm support
 - [+] basic support
 - login if needed

Code
----
* Cleanup entire code
* FarGlobal class:
 - [+] InputBox support
  + [+] Base support
  + invesigate and implement history support
  + Buttons and additional flags
* String utils
 - [+] tokenize, trim

Other
-----
* Choose open-source license
* README.md
 - purpose of this plugin
* Release and x64 builds
* Plugring release
* FAR Manager
 - fill a ticket with '\n' in input box and provide a patch request
