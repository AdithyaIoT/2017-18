# Download the Python helper library from twilio.com/docs/python/install
import sys
from twilio.rest import Client

# Your Account Sid and Auth Token from twilio.com/user/account
account_sid = "AC54550c68f6f953c5d885a5e4caba988c"
auth_token = "e7940e01a4b761c7a1b1b3d3f68525cc"
client = Client(account_sid, auth_token)
cellphone = sys.argv[1]
twilio_phone_number = "+17372011402"
call = client.calls.create(
    to=cellphone,
    from_=twilio_phone_number,
    url="http://510f3511.ngrok.io/AdiHealthBand/TwilioInstruction.xml"
)

print(call.sid)