

import boto3
# shellycarme@gmail.com
#YH33keva
#pip install boto3

import numpy as np

def sendSms(phoneNumber, sum):
    
    sns = boto3.client('sns', aws_access_key_id="AKIAJNNUISWTPB424MWA",  aws_secret_access_key="55nM6Ipnn9Gh4mPg1/1vOfJpVEcURiz4JoLj3mQJ",   region_name="us-east-1")
    # Send your sms message.
    sns.publish(
        PhoneNumber=phoneNumber,
        Message="total sum of money is "+ sum

    )
    print('phoneNumber ' + phoneNumber)


if __name__ == '__main__':
    import sys, getopt

    phoneNumber = sys.argv[1]
    sum = sys.argv[2]

    sendSms(phoneNumber, sum)
    

   
