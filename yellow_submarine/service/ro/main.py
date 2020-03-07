#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import uuid
import signal
import base64
from Crypto.Cipher import AES
from Crypto.PublicKey import RSA
import os
import random
import hashlib

banner = '''
 __ __     _ _              _____     _                 _         
|  |  |___| | |___ _ _ _   |   __|_ _| |_ _____ ___ ___|_|___ ___ 
|_   _| -_| | | . | | | |  |__   | | | . |     | .'|  _| |   | -_|
  |_| |___|_|_|___|_____|  |_____|___|___|_|_|_|__,|_| |_|_|_|___|
                                                                  

Welcome to the Yellow Submarine, the Noah's ark in the twenty-first century.
Your secrets will be securely stored deep in the sea.
'''

def number_to_base64(n, size):
  assert n < 256**size
  return base64.b64encode(format(n, '0%dx' % (2*size)).decode('hex'))

def sign(key, message):
  message = base64.b64decode(message)
  m = int(message.encode('hex'), 16)
  n, d = key.n, key.d
  s = pow(m, d, n)
  signature = format(s, '0512x').decode('hex')
  return base64.b64encode(signature)

def verify(key, message, signature):
  return sign(key, message) == signature

def pad(plaintext):
  padding = 16 - len(plaintext) % 16
  return plaintext + chr(padding) * padding

def timeout_handler():
  raise EOFError

def main():
  signal.signal(signal.SIGALRM, timeout_handler)
  signal.alarm(60)

  print(banner)
  key_id, key = None, None
  while True:
    try:
      action = raw_input('yellow-submarine$ ')
      if action == 'keygen':
        challenge = format(random.getrandbits(16), '04x')
        prefix = format(random.getrandbits(32), '08x')
        print('Please solve the given proof-of-work challenge: %s|%s.' % (challenge, prefix))
        resp = raw_input('> ')
        if not hashlib.sha256(prefix + resp).hexdigest().startswith(challenge): raise Exception('Incorrect proof-of-work response')
        key = RSA.generate(2048)
        key_id = uuid.uuid4()
        with open('signer_%s.key' % key_id, 'w') as f:
          f.write(key.exportKey())
        print('n = %s' % number_to_base64(key.n, 2048//8))
        print('e = %s' % number_to_base64(key.e, 2048//8))
      elif action == 'store':
        if not key or not key_id: raise Exception('Key not generated')
        print('Please send me a filename')
        filename = raw_input('> ').strip()
        print('Please tell me your secret (base64 encoded). It will be stored very securely!')
        plaintext = base64.b64decode(raw_input('> '))
        if len(plaintext) > 1000: raise Exception('Secret too long')
        padded_plaintext = pad(plaintext)
        k = format(random.getrandbits(128), '032x').decode('hex')
        cipher = AES.new(k, AES.MODE_CBC, '\x00' * 16)
        ciphertext = cipher.encrypt(padded_plaintext)
        with open('data_%s' % filename, 'w') as f: f.write(ciphertext)
        with open('data_%s.key' % filename, 'w') as f: f.write(k)
        cmd_f = base64.b64encode('cat data_%s' % filename)
        sig_f = sign(key, cmd_f)
        cmd_kf = base64.b64encode('cat data_%s.key' % filename)
        sig_kf = sign(key, cmd_kf)
        token = '%s|%s|%s|%s|%s' % (key_id, cmd_f, cmd_kf, sig_f, sig_kf)
        print('Secret stored. You can use the following token to extract the secret securely later:')
        print(token)
      elif action == 'read':
        print('Please send me the token to read the file')
        token = raw_input('> ')
        key_id, cmd_f, cmd_kf, sig_f, sig_kf = token.split('|')
        with open('signer_%s.key' % key_id) as f: key = RSA.importKey(f.read())
        if not verify(key, cmd_f, sig_f) or not verify(key, cmd_kf, sig_kf): raise Exception('Invalid signature')
        cmd_f = base64.b64decode(cmd_f)
        cmd_kf = base64.b64decode(cmd_kf)
        with os.popen(cmd_f) as f: ciphertext = f.read()
        with os.popen(cmd_kf) as f: data_key = f.read()
        cipher = AES.new(data_key, AES.MODE_CBC, '\x00' * 16)
        plaintext = cipher.decrypt(ciphertext)
        print('Hey. This is your secret:')
        print(base64.b64encode(plaintext))
      elif action == 'exit':
        break
    except EOFError:
      break
    except Exception as err:
      print('\033[031mError: %s\033[0m' % err)
  print('Bye!')

if __name__ == '__main__':
  main()

'''
Note:
* The token format must not be changed.
* Proof-of-work difficulty must be 2^16 (i.e. 4 hex characters).
'''
