# NoSSL patch for the WiiU FW version 5.5.5

5.5.5 updated a verification callback in IOS-NSEC, so the function we were patching was shifted a bit.

This code is the patch basically (it's a physical memory write):
```C
*(uint32_t*)(0xE1019F78 - 0xE1000000 + 0x12BC0000) = 0xE3A00001;
```

It's patching:

```C
// OpenSSL 1.0.0f 4 Jan 2012
// @ E1019F50
int ssl_verify_cert_chain(SSL *s,STACK_OF(X509) *sk);
```
