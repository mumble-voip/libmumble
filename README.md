# libmumble

## Introduction

Official Mumble library, written in C++17.

Designed from scratch with performance, flexibility and security in mind.

Supposed to work on as many operating systems as possible, namely:

- Windows ([10 1803](https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows) and higher)
- Linux
- BSD (including macOS)

Allows to easily implement both client and server, without any restrictions.

### Performance

Please note that no benchmarks have been written yet. It's entirely possible there are bottlenecks, which we are ready to solve as soon as found.

Particular care was taken into making the entire codebase as efficient as possible, but let's focus on the protocol management:

#### TCP

Always used as the control channel.  
It can also tunnel voice packets, which is required when dealing with strict network environments; for example, a lot of universities only allow TCP traffic.

Since it's a "stream" protocol, multiple syscalls can be required in order to send complete data.  
The `Connection` class hides all of that for you, behind neat functions.

If you ask the library to drive multiple connections, the integrated thread pool will take care of distributing work.  
By default the pool uses as many threads as your machine has to offer, but you can of course tune the number. This is especially important when you're running heavy multi-threaded applications.

#### UDP

Should always be used as the voice channel, when possible.

UDP is generally preferred over TCP for low latency applications because it's not affected by the overhead of the latter.  
In poor words, it adds the bare minimum required to send data over the wire.

Since it's a "block" protocol, complete data is generally sent with a single syscall, in the form of a so-called "datagram".

It's referred to as "connectionless" because not stateful: there is no explicit relation between peers.  
In practice, this means that:

- You have to specify the destination endpoint (IP address and port) when sending data.
- You have to determine the source peer yourself; this is usually implemented in the form of an hash table, for fast lookup performance.

The [server example](src/example/ExampleServer) demonstrates how to set up secure communication and subsequently caching.

As for the client: in a common scenario there's only one server connected, meaning that a single peer is expected at all times.  
In the case you want your client to connect to multiple servers concurrently, you would have to adapt the code provided in the server example.

### Flexibility

[Examples](src/example) were written alongside the library, to make sure that there would be no shortcomings in the final result.

An ideal API, in addition to making the work easier for its consumers, should also provide enough freedom to alter the internal behavior.  
In fact, you can go as far as driving the connection(s) yourself, perhaps with your own dispatcher.

Every single public class, with the exception of protocol-specific ones (see [`Message.hpp`](include/mumble/Message.hpp) and [`Pack.hpp`](include/mumble/Pack.hpp)), can be used even if you're planning to write your own protocol or an unrelated application overall.

The power is in your hands!

### Security

#### Dependencies

A Mumble library is something we've wanted to create for years, but there's one event in particular that really pushed our desire for it: mumble-voip/mumble#3679

Basically, we decided to gradually reduce our dependence on Qt until it's only used for the UI, significantly diminishing the potential attack surface: much less complexity involved and we have full control over critical code.

Also, it became clear the less external dependencies we relied on the better.  
And thus, the full list for this library is:

- [Boost](https://www.boost.org) (span and threads)
- [OpenSSL](https://www.openssl.org) (cryptography and TLS/SSL)
- [Opus](https://opus-codec.org) (audio codec)
- [Protobuf](https://developers.google.com/protocol-buffers) (protocol serialization)
- [quickpool](https://github.com/tnagler/quickpool) (thread pool)
- [wepoll](https://github.com/piscisaureus/wepoll) (epoll for Windows)

#### Memory

Written in modern C++ since the very beginning, the codebase makes heavy use of awesome stuff such as smart pointers.

Also, most of the time testing was performed by running the examples through Valgrind, pretty much guaranteeing no corruptions and/or leaks.

The API makes eventual objects (e.g. instantiation of `Connection`) easy to manage in terms of lifetime and ownership.

#### OpenSSL

OpenSSL is the most used library/framework for cryptography and TLS/SSL, but most people who worked with it agree that both API and documentation could be much better.  
https://github.com/mumble-voip/libmumble/commit/95c224e8f0b470bab0f366d78b1a48a2eaa69803 is a great example of that.

A lot of effort was poured into writing efficient classes that expose consistent methods, greatly simplifying the various operations that are required and/or recommended to establish and maintain a secure Mumble session.

## API

### `lib` namespace

Used to initialize/deinitialize the library and retrieve its version.

Please note that calling `init()` and `deinit()` is only required on Windows, for the network subsystem.  
However, we strongly recommend to do it for other platforms as well, just in case something will require global initialization in the future.

#### version()

Returns the library version as a `Version` object.

#### init()

The library is initialized, if the internal counter is 0 when calling the function.  
An atomic increase is always performed, except in case of error.

A result code is returned.

#### deinit()

An internal counter is decreased atomically and, if 0 is reached, the library is deinitialized.

A result code is returned.

#### initCount()

Reads the internal counter atomically and returns the value.

---

### Base64

Right now this is a simple interface to OpenSSL's Base64 API, but a rewrite is planned.  
The class will probably become header-only.

An object can be constructed through:

- None

#### decode()

Decodes the specified Base64 data into the specified buffer.

Returns the number of bytes written.

#### encode()

Encodes the specified binary data into the specified buffer.

Returns the number of bytes written.

### Cert

Used to hold a certificate and retrieve essential info.

An object can be constructed through:

- None
- Copy
- Move
- OpenSSL handle
- DER format certificate
- PEM format certificate

#### handle()

Returns the OpenSSL handle.

#### der()

Returns the certificate in DER format.

#### pem()

Returns the certificate in PEM format.

#### publicKey()

Returns the public key as a `Key` object.

#### since()

Returns a time point ([`std::chrono::time_point`](https://en.cppreference.com/w/cpp/chrono/time_point)) object representing the validity start date and time.

#### until()

Returns a time point ([`std::chrono::time_point`](https://en.cppreference.com/w/cpp/chrono/time_point)) object representing the validity end date and time.

#### isAuthority()

Returns whether a CA (i.e. can issue other certificates).

#### isIssuer()

Returns whether the specified certificate was issued through this one.

#### isSelfIssued()

Returns whether self-signed.

#### subjectAttributes()

Returns the certificate attributes.

Please note that the returned [`std::map`](https://en.cppreference.com/w/cpp/container/map) uses a read-only string reference ([`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view)) as key.  
Only the value can be edited!

#### issuerAttributes()

Returns the certificate issuer attributes.

Please note that the returned [`std::map`](https://en.cppreference.com/w/cpp/container/map) uses a read-only string reference ([`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view)) as key.  
Only the value can be edited!

### Connection

Represents a TLS/SSL connection.

An object can be constructed through:

- Move
- Socket handle

#### operator()

Establishes a TLS/SSL session and returns the result code.

Please note that, depending on both TLS/SSL and TCP behavior, the session may die if you don't process received data or send any in time!

#### socketHandle()

Returns the socket handle.

#### endpoint()

Returns an `Endpoint` object representing the local IP address and port.

#### peerEndpoint()

Returns an `Endpoint` object representing the remote IP address and port.

#### cert()

Returns the local certificate chain as a list of `Cert` objects.

#### peerCert()

Returns the remote certificate chain as a list of `Cert` objects.

#### setCert()

Sets the passed `Cert` list as local certificate chain, using the passed `Key` object.

This function should be called right before establishing a TLS/SSL session.

#### process()

Processes any pending data that is received and returns the result code.

When `wait` is set to `false`, the function returns even if the read operation is incomplete.

A wait operation can be interrupted prematurely by returning `true` in the passed predicate.

#### write()

Writes the provided data and returns the result code.

When `wait` is set to `false`, the function returns even if the write operation is incomplete.

A wait operation can be interrupted prematurely by returning `true` in the passed predicate.

### Crypt

Used for encryption & decryption operations.

An object can be constructed through:

- None
- Move

#### handle()

Returns the OpenSSL handle.

#### cipher()

Returns the cipher name as a read-only string reference ([`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view)).

#### setCipher()

Sets the cipher by name.

Returns whether the operation succeeded.

#### blockSize()

Returns the current cipher's block size.

#### keySize()

Returns the current key's size.

#### nonceSize()

Returns the current nonce's size

#### key()

Returns the current key as a read-only vector of bytes.

#### genKey()

Generates a new key securely and returns it as a vector of bytes.

#### setKey()

Sets the specified key.

Returns whether the operation succeeded.

#### nonce()

Returns the current nonce as a read-only vector of bytes.

#### genNonce()

Generates a new nonce securely and returns it as a vector of bytes.

#### setNonce()

Sets the specified nonce.

Returns whether the operation succeeded.

#### usesPadding()

Returns whether padding is enabled.

#### togglePadding()

Enables or disables padding, depending on the passed flag.

Returns whether the operation succeeded.

#### reset()

Fully resets the OpenSSL context, recommended when rekeying or switching ciphers.

Returns whether the operation succeeded.

#### decrypt()

Decrypts the specified data into the specified buffer.

For AEAD operation you should specify the tag and AAD (additional authenticated data) as well.

Returns the number of bytes written.

#### encrypt()

Encrypts the specified data into the specified buffer.

For AEAD operation you probably want to specify the AAD (additional authenticated data) as well.  
In that case `tag` should point to a buffer that will receive the produced tag.

Returns the number of bytes written.

### CryptOCB2

This is essentially a special clone of `Crypt` providing support for our OCB2 implementation.

Even if audited, you should rely on `Crypt` instead when possible; it allows to use better ciphers such as ChaCha20-Poly1305 and AES in GCM mode.

For reference, see mumble-voip/mumble#4824.

### Endian

Swaps the byte order on 16, 32 and 64 bits integers.

#### isBig()

Returns whether the library is big endian (the return value is computed at compile time).

#### isLittle()

Returns whether the library is little endian (the return value is computed at compile time).

#### swap()

Returns the passed integer value, with swapped byte order.

#### toNetwork()

Returns the passed integer value; the byte order is swapped if the library is little endian.

#### toHost()

Returns the passed integer value; the byte order is swapped if the library is little endian.

### Hash

Used to compute hashes.

An object can be constructed through:

- None
- Move

#### operator()

Computes the hash for the provided data and stores it in the specified buffer.

Returns the number of bytes written.

#### handle()

Returns the OpenSSL handle.

#### type()

Returns the hash type (algorithm) name as a read-only string reference ([`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view)).

#### setType()

Sets the hash type (algorithm) by name.

Returns whether the operation succeeded.

#### blockSize()

Returns the current hash type's block size.

#### reset()

Fully resets the OpenSSL context, recommended when switching algorithms.

Returns whether the operation succeeded.

### IP

Used to hold an IP address and retrieve essential info.

IPv4 addresses are stored in the so-called "IPv4-mapped IPv6 address" format ([RFC 4291](https://www.rfc-editor.org/rfc/rfc4291)).  
For example, `127.0.0.1` would become `::ffff:127.0.0.1`.

An object can be constructed through:

- None
- Copy
- Vector of bytes
- String
- `sockaddr_in6` object

#### v6()

Returns a reference to the full vector of bytes.

The pointed data is read-only if the object is `const`.

#### v4()

Returns a reference to the last 4 slots in the vector of bytes.

The pointed data is read-only if the object is `const`.

#### isV6()

Returns whether the stored bytes represent an IPv6 address.

#### isV4()

Returns whether the stored bytes represent an IPv4 address.

#### isWildcard()

Returns whether the stored bytes represent:

- The wildcard IPv6 address (`::`)
- The wildcard IPv4 address (`::ffff:0.0.0.0`)

#### text()

Returns a string representing the stored bytes.

Please note that for IPv4 the format most people are familiar with is used: `255.255.255.255` as opposed to `::ffff:255.255.255.255`.

#### toSockAddr()

Copies the stored bytes into the passed `sockaddr_in6` object.

### Key

Used to hold a private or public key.

An object can be constructed through:

- None
- Copy
- Move
- OpenSSL handle
- PEM format key

#### handle()

Returns the OpenSSL handle.

#### isPrivate()

Returns whether this is a private key.

#### pem()

Returns the key in PEM format.

### Message

This is essentially a collection of `structs` representing a _proper_ mapping of our protocol messages.  
The rationale is:

1. Avoiding a direct dependency on the specific Protobuf version for the library's consumers.
2. Avoiding the overall insanity that is unfortunately present in the messages generated by the Protobuf compiler, such as raw bytes being represented as an `std::string`...

#### protocol()

Returns the protocol type in the form of a `Protocol` enum value; can be either:

- [0] Unknown
- [1] TCP
- [2] UDP

#### type()

Returns the message type in the form of a `Type` enum value; matches both name and value that are in the proto file(s).

#### text()

Returns a read-only string reference ([`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view)) representing the name of the type; matches the name that is in the proto file(s).

### OpusDecoder

Used to decode Opus packets.

An object can be constructed through:

- Move
- Number of channels

#### operator()

Decodes the specified data into the specified buffer.

Set `decodeFEC` to `true` if you want Opus to take the forward error correction info into account.

#### init()

Initializes the Opus context with the specified sample rate.

A result code is returned.

#### reset()

Resets the Opus context state.

A result code is returned.

#### channels()

Returns the number of channels the context was initialized for.

#### sampleRate()

Returns the sample rate the context was initialized for.

#### inDTX()

Returns whether the context is in DTX mode.

It is when last frame was either ignored or a comfort noise frame.

#### usesPhaseInversion()

Returns whether phase inversion is enabled.

#### togglePhaseInversion()

Enables or disables phase inversion, depending on the passed flag.

Returns whether the operation succeeded.

#### packetSamples()

Returns the number of samples in the specified Opus packet.

### OpusEncoder

Used to encode Opus packets.

An object can be constructed through:

- Move
- Number of channels

#### operator()

Encodes the specified data into the specified buffer.

#### init()

Initializes the Opus context with the specified sample rate and preset.

A result code is returned.

#### reset()

Resets the Opus context state.

A result code is returned.

#### channels()

Returns the number of channels the context was initialized for.

#### sampleRate()

Returns the sample rate the context was initialized for.

#### preset()

Returns the context's current preset.

#### setPreset()

Sets the specified preset in the context.

Returns whether the operation succeeded.

#### bitrate()

Returns the context's current bitrate.

#### setBitrate()

Sets the specified bitrate in the context.

Special values are:

- `0` to set the default bitrate.
- The highest value that can be stored in an unsigned 32 bit integer to set the maximum bitrate supported.  
  You can use the `UINT32_MAX` macro or, ideally, `std::numeric_limits< std::uint32_t >::max()`.

Returns whether the operation succeeded.

#### inDTX()

Returns whether the context is in DTX mode.

It is when last frame was either ignored or a comfort noise frame.

#### usesPhaseInversion()

Returns whether phase inversion is enabled.

#### togglePhaseInversion()

Enables or disables phase inversion, depending on the passed flag.

Returns whether the operation succeeded.

#### usesVBR()

Returns whether variable bitrate is enabled.

#### toggleVBR()

Enables or disables variable bitrate, depending on the passed flag.

Returns whether the operation succeeded.

### Pack

Used to serialize and deserialize Protobuf messages.

Its internal buffer is a vector of bytes and can hold a serialized message, ready to be sent.  
The first 6 bytes of the buffer represent the header as a `NetHeader` object.

An object can be constructed through:

- Copy
- Move
- `Message` (native or Protobuf's)
- `NetHeader` (+ data size if UDP)

Allocating extra space for the internal buffer is possible and it's what `Peer` does internally for UDP, to improve efficiency: the `Pack` object is recycled for every packet received.

#### operator()

Deserializes the held message into the `Message` object that is passed via reference.

`dataSize` is only required to be specified when the vector of bytes is not fully occupied.  
The Protobuf library's deserializer fails if it encounters extra bytes that are not part of the message.

Returns whether the operation succeeded.

#### buf()

Returns a reference to the full vector of bytes.

This is generally what you want to use when sending messages.

The pointed data is read-only if the object is `const`.

#### data()

Returns a reference to the vector of bytes, after skipping the `NetHeader` object.

The pointed data is read-only if the object is `const`.

#### header()

Returns a reference to the `NetHeader` object.

The pointed data is read-only if the object is `const`.

#### type()

Returns the message type specified in the `NetHeader` object as a `Type` enum value.

### Peer

This is the networking core of your client or server.

#### connect()

Establishes a connection (TCP) to the specified endpoint (`peerEndpoint`).

`endpoint` can be filled in order to use a specific source endpoint.  
Especially useful when you have multiple Internet connections and want to force a specific one.

Returns an [`std::pair`](https://en.cppreference.com/w/cpp/utility/pair) containing the result code and a socket handle (also called "file descriptor").  
The socket is guaranteed to be valid if the code indicates success.

At this point the handle is yours and you can do anything you want with it.  
You generally want to create a `Connection` object, in order to establish and maintain a TLS/SSL connection.

#### startTCP()

Starts the TCP engine; the internal thread pool will spawn the specified number of threads.  
If said number is `0`, there will be a thread for each physical one advertised by the system.

You'll receive new events through the passed `FeedbackTCP` object.  
If `bindTCP()` was called, you will be notified of any connection requests.

A result code is returned.

#### stopTCP()

Stops the TCP engine: all related threads are shut down and you'll no longer receive related events.

A result code is returned.

#### startUDP()

Starts the UDP engine; the initial buffer size will be the specified number of bytes.
If said number is `0`, a sane default will be used.

You'll receive new events through the passed `FeedbackUDP` object.

A result code is returned.

#### stopUDP()

Stops the UDP engine: all related threads are shut down and you'll no longer receive related events.

A result code is returned.

#### bindTCP()

Only call this if you want to listen for connections (i.e. a server).

A result code is returned.

#### unbindTCP()

Closes the TCP socket.

A result code is returned.

#### bindUDP()

Call this if you want to receive and/or send UDP packets (recommended).

A result code is returned.

#### unbindUDP()

Closes the UDP socket.

A result code is returned.

#### addTCP()

Adds the specified connection to the TCP engine: you'll receive events through the `Connection` object's feedback interface.

A result code is returned.

#### delTCP()

Removes the specified connection from the TCP engine: you'll no longer receive related events.

A result code is returned.

#### sendUDP()

Sends the provided data to the specified endpoint, using the socket created during the `bindUDP()` call.

A result code is returned.
