//
//  ViewController.m
//  NaoCar Remote
//
//  Created by gael on 15/03/13.
//  Copyright (c) 2013 NaoCar. All rights reserved.
//

#import "ViewController.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.directionController = [[JoystickViewController alloc] init];
    self.directionController.initialPosition = CGPointMake(0, 620);
    self.directionController.delegate = self;
    [self.view addSubview: self.directionController.view];
    
    // Init application states
    self.naoAvailable = false;
    self.direction = Front;
    self.shift = Frontwards;

    // Init Bonjour to retrieve the Remote server
    self.bonjourServiceBrowser = [[NSNetServiceBrowser alloc] init];
    self.bonjourServiceBrowser.delegate = self;
    [self.bonjourServiceBrowser searchForServicesOfType:@"_http._tcp" inDomain:@""];
    
    // Create a dictionnary to hold command response datas
    self.commandResponseDatas = [NSMutableDictionary dictionary];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)dealloc {
    [self sendCommand:@"/end"];
}

#pragma mark Bonjour relative functions to retrieve server IP and port

-(void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing {
    if ([aNetService.name isEqualToString:BONJOUR_SERVICE_NAME]) {
        aNetService.delegate = self;
        self.bonjourService = aNetService;
        [aNetService resolveWithTimeout:0];
    }
}

-(void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing {
    if ([aNetService.name isEqualToString:BONJOUR_SERVICE_NAME]) {
        self.naoAvailable = NO;
        self.bonjourService = nil;
        self.naoUrl = nil;
        self.waitingLabel.hidden = NO;
        self.streamImageView.image = [UIImage imageNamed:@"background.png"];
    }
}

-(void)netServiceDidResolveAddress:(NSNetService *)sender {
    self.naoAvailable = YES;
    const struct sockaddr_in* addr =
        (const struct sockaddr_in*)[(NSData*)[sender.addresses objectAtIndex:0] bytes];
    NSString* ip = [NSString stringWithCString:inet_ntoa(addr->sin_addr) encoding:NSASCIIStringEncoding];
    int port = sender.port;
    self.naoUrl = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@:%d", ip, port]];
    self.naoIp = ip;
    NSLog(@"NaoCar Remote Server found at %@:%d", ip, port);
    self.waitingLabel.hidden = YES;
    [self sendCommand:@"/begin"];
    [self sendCommand:@"/get-stream-port"];
}

#pragma mark Touch events handling

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch* touch in touches) {
        if ([touch locationInView:self.view].x < self.view.frame.size.width / 2) {
            [self.directionController touchesBegan:touches withEvent:event];
        } else {
            [self.speedController touchesBegan:touches withEvent:event];
        }
    }
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch* touch in touches) {
        [self.directionController touchesMoved:touches withEvent:event];
        [self.speedController touchesMoved:touches withEvent:event];
    }

}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch* touch in touches) {
        [self.directionController touchesEnded:touches withEvent:event];
        [self.speedController touchesEnded:touches withEvent:event];
    }

}

#pragma mark User input

-(void)joystick:(id)joystick valueChanged:(CGPoint)value {
    if (value.x > DIRECTION_JOYSTICK_TESHOLD
        && _direction != Right) {
        _direction = Right;
        [self sendCommand:@"/turn-right"];
        
    } else if (value.x < -DIRECTION_JOYSTICK_TESHOLD
               && _direction != Left) {
        _direction = Left;
        [self sendCommand:@"/turn-left"];
    } else if (ABS(value.x) < DIRECTION_JOYSTICK_TESHOLD
               && _direction != Front) {
        _direction = Front;
        [self sendCommand:@"/turn-front"];
    }
}

- (IBAction)toggleShift:(id)sender {
    if (_shift == Frontwards) {
        [self.shiftButton setBackgroundImage:[UIImage imageNamed:@"shift-backwards.png"] forState:UIControlStateNormal];
        _shift = Backwards;
        [self sendCommand:@"/upshift"];
    } else {
        [self.shiftButton setBackgroundImage:[UIImage imageNamed:@"shift-frontwards.png"] forState:UIControlStateNormal];
        _shift = Frontwards;
        [self sendCommand:@"/downshift"];
    }
}

- (IBAction)pushPedal:(id)sender {
    [self.pedalButton setBackgroundImage:[UIImage imageNamed:@"pedal-pushed.png"] forState:UIControlStateNormal];
    [self sendCommand:@"/push-pedal"];
}

- (IBAction)releasePedal:(id)sender {
    [self.pedalButton setBackgroundImage:[UIImage imageNamed:@"pedal.png"] forState:UIControlStateNormal];
    [self sendCommand:@"/release-pedal"];
}

#pragma mark Network handling

-(void)sendCommand:(NSString *)command {
    if (!self.naoAvailable)
        return ;
    NSURL* commandUrl = [self.naoUrl URLByAppendingPathComponent:command];
    NSURLRequest* request = [NSURLRequest requestWithURL:commandUrl];
    // Execute request
    NSURLConnection* connection = [NSURLConnection connectionWithRequest:request delegate:self];
    if (connection) {
        [self.commandResponseDatas setObject:[NSMutableData data] forKey:[NSNumber numberWithLong:(long)connection]];
    }
}

-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    NSMutableData* responseData = [self.commandResponseDatas objectForKey:[NSNumber numberWithLong:(long)connection]];
    if (!responseData)
        return ;
    [responseData appendData:data];
}

-(void)connectionDidFinishLoading:(NSURLConnection *)connection {
    NSNumber* connectionKey = [NSNumber numberWithLong:(long)connection];
    NSMutableData* responseData = [self.commandResponseDatas objectForKey:connectionKey];
    if (!responseData)
        return ;
    NSString* response = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
    
    if ([response hasPrefix:@"stream-port"]) {
        short port = [[response substringFromIndex:12] intValue];
        [self beginStreaming:port];
    }
    
    // Delete response data
    [self.commandResponseDatas removeObjectForKey:connectionKey];
}

#pragma mark Stream receiving

-(void)beginStreaming:(uint16_t)port {
    self.streamSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
    NSError* error = nil;
    if (![self.streamSocket connectToHost:self.naoIp onPort:port error:&error] || error) {
        self.streamSocket = nil;
    }
}

-(void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(uint16_t)port {
    [self readStreamImageSize];
}

-(void)readStreamImageSize {
    [self.streamSocket readDataToLength:sizeof(uint64_t) withTimeout:-1 tag:STREAM_READ_IMAGE_SIZE];
}

-(void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag {
    if (tag == STREAM_READ_IMAGE_SIZE) {
        const uint64_t* size = [data bytes];
        [self.streamSocket readDataToLength:*size withTimeout:-1 tag:STREAM_READ_IMAGE_DATA];
    } else if (tag == STREAM_READ_IMAGE_DATA) {
        UIImage* image = [UIImage imageWithData:data];
        self.streamImageView.image = image;
        [self readStreamImageSize];
    }
}

@end
