# Nimble - Tiramisu patches for Pretendo 

[![Pretendo network logo](https://github.com/PretendoNetwork/website/raw/master/public/assets/images/opengraph/opengraph-image.png)](https://pretendo.network)

Nimble is a Tiramisu setup module that patches various Nintendo Network URLs on a Wii U to use [Pretendo Network](https://pretendo.network) instead; as well as applying various in-game URL and security patches. It also (for the time being) bypasses SSL verification in most cases.

## Dependencies

The only supported configuration for Nimble is a console with Tiramisu and autoboot/coldboot. For Aroma sytems, see [Inkay](https://github.com/PretendoNetwork/Inkay).

## Safety

Nimble's patches are all temporary, and only applied in-memory without modifying your console. The SSL patch, while also temporary, could reduce the overall security of your console while active - this is because it no longer checks if a server is verified. However, this does *not* apply to the Internet Browser, where SSL still works as expected.

## TODO
See [Issues](https://github.com/PretendoNetwork/Nimble/issues).
