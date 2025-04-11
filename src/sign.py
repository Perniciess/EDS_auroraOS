#!/usr/bin/env vpython3
# *-* coding: utf-8 *-*
import sys
import datetime
from cryptography.hazmat import backends
from cryptography.hazmat.primitives.serialization import pkcs12
from endesive.pdf import cms

def main(cert_path, input_pdf, output_pdf):
    date = datetime.datetime.now(datetime.UTC) - datetime.timedelta(hours=12)
    date = date.strftime("D:%Y%m%d%H%M%S+00'00'")
    
    dct = {
        "aligned": 0,
        "sigflags": 3,
        "sigflagsft": 132,
        "sigpage": 0,
        "signform": True,
        "sigfield": "Signature",
        "signature": 'Signed field!',
        "text": {
            'wraptext': True,
            'fontsize': 12,
            'textalign': 'left',
            'linespacing': 1.2,
        },
        "contact": "mak@trisoft.com.pl",
        "location": "Szczecin",
        "signingdate": date,
        "reason": "Dokument podpisany cyfrowo aą cć eę lł nń oó sś zż zź",
        "password": "1234",
    }

    with open(cert_path, "rb") as fp:
        p12 = pkcs12.load_key_and_certificates(
            fp.read(), b"1234", backends.default_backend()
        )

    datau = open(input_pdf, "rb").read()
    datas = cms.sign(datau, dct, p12[0], p12[1], p12[2], "sha256")
    
    with open(output_pdf, "wb") as fp:
        fp.write(datau)
        fp.write(datas)

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python sign_pdf.py <cert.p12> <input.pdf> <output.pdf>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2], sys.argv[3])