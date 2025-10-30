# no-more-cssource-shellcodes

Shellcode vulnerability fixes for Counter-Strike: Source v34.

## What?

If you didn't know, in Counter-Strike: Source v34 there are a number of ways a server can download executable files to a client or even perform an RCE attack via an audio packet. This project closes all of those vulnerabilities.

## Why?

It appears that v34 still remains a popular client version. I suddenly remembered that I had made these patches some time ago, and simply decided to publish them, compiling everything into a separated project.

Some existing No-Steam builds already include these fixes (guess whose), but from now on their source code is open and you can admire the ingenuity of the people who invented ways to inject a product into the client from the server side.

## How?

The project is not designed for easy integration into a game (as usual) but is intended for users who know what they are doing. Integrate it into your projects and enjoy protection against DLL injections.

## License

This project is distributed under the 2-Clause BSD License.