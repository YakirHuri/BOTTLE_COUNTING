

import boto3
# Nviner1@gmail.com
#Net@nel79456123

#pip3 install boto3
#python3 main.py




def sendSms(phoneNumber, sum):
    
    sns = boto3.client('sns', aws_access_key_id="AKIAIYHKIJ3XYPE35PMA",  aws_secret_access_key="6cUe7PqLsZt0wMyjUdNbkuFLhbF/uBhZosmzeQZR",   region_name="us-east-1")
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
    