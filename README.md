# app-password-manager

## Quick summary

Password Manager application for Ledger Nano S and Nano X

This application demonstrates a Password Manager implemented with no support from the host - the passwords are typed from the Nano S interacting as a keyboard to the connected computer / phone.

## Usage

To create a password:
   * Choose which kind of characters you want in this password (lowercase, uppercase, numbers, dashes, extra symbols)
   * Enter a nickname for the new entry (for instance, "wikipedia.com").
The device then derives a deterministic password from the device's seed and this nickname.

To type a password, just select it in your list of password.
In the settings, the user can also choose which keyboard the device should emulate when typing a password (Qwerty, International Qwerty or Azerty).

## Backup

As passwords are deterministically derived, it's not a problem if you loose your device, as long as you remember the password nicknames and you still have you device recovery phrase to set up again the this app on a new device.

Same applies when updating the device firmware, the list of password nicknames won't be restore automatically, so make sure to note them somewhere.

These nicknames are not confidential (meaning, someone who finds them will not be able to retrieve your passwords without your recovery phrase), so you don't have to hide them like you did with your recovery phrase.

## Password generation mechanism

* Metadatas are SHA-256 hashed

* The SHA-256 components are turned into 8 big endian uint32 | 0x80000000

* A private key and chain code are derived for secp256k1 over 0x80505744 / the path computed before

* The private key and chain code are SHA-256 hashed, the result is used as the entropy to seed an AES DRBG

* A password is generated by randomly choosing from a set of characters using the previously seeded DRBG

## Future work

This release is an early alpha - among the missing parts :

   * Support of different password policies mechanisms

   * Metadatas backup, with a python script or even a nice web GUI

   * Offline recovery program

## Credits

This application uses

  * MBED TLS AES DRBG implementation (https://tls.mbed.org/ctr-drbg-source-code)
