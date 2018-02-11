import sys
from twilio.rest import Client
account_sid = "AC54550c68f6f953c5d885a5e4caba988c"
auth_token = "e7940e01a4b761c7a1b1b3d3f68525cc"
message=sys.argv[1]
twilio_phone_number = "+17372011402"
cellphone = sys.argv[2]
client = Client(account_sid, auth_token)
client.messages.create(to=cellphone, from_=twilio_phone_number, body=message)

