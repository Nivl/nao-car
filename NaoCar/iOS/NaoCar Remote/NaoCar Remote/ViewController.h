//
//  ViewController.h
//  NaoCar Remote
//
//  Created by gael on 15/03/13.
//  Copyright (c) 2013 NaoCar. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreMotion/CoreMotion.h>

#import "GCDAsyncSocket.h"
#import "JoystickViewController.h"

typedef enum {
    Frontwards,
    Backwards
} Shift;

typedef enum {
    Front,
    Left,
    Right
} Direction;

typedef enum {
    ControlJoystick = 0,
    ControlAccelerometer = 1
} ControlMode;

typedef enum {
    ViewCamera1 = 0,
    ViewCamera2 = 1,
    ViewOpenCV = 2
} ViewMode;

#define DIRECTION_JOYSTICK_TESHOLD 0.5
#define DIRECTION_ACCELEROMETER_TESHOLD 0.3
#define BONJOUR_SERVICE_NAME @"nao-car"

#define STREAM_READ_IMAGE_SIZE 0
#define STREAM_READ_IMAGE_DATA 1

@interface ViewController : UIViewController<JoystickViewControllerDelegate, NSNetServiceBrowserDelegate, NSNetServiceDelegate, NSURLConnectionDataDelegate, GCDAsyncSocketDelegate>

-(void)accelerometerValueChanged:(CMAccelerometerData*)accelerometerData;

- (IBAction)toggleShift:(id)sender;
- (IBAction)pushPedal:(id)sender;
- (IBAction)releasePedal:(id)sender;

-(void)sendCommand:(NSString*)command;

-(void)beginStreaming:(uint16_t)port;
-(void)readStreamImageSize;

@property JoystickViewController* directionController;
@property CMMotionManager* motionManager;

@property (nonatomic) BOOL naoAvailable;
@property (nonatomic) NSURL* naoUrl;
@property (nonatomic) NSString* naoIp;
@property (nonatomic) Direction direction;
@property (nonatomic) Shift shift;
@property (nonatomic) ControlMode controlMode;
@property (nonatomic) ViewMode viewMode;

@property (nonatomic) NSNetServiceBrowser* bonjourServiceBrowser;
@property (nonatomic) NSNetService* bonjourService;

@property (nonatomic) NSMutableDictionary* commandResponseDatas;

@property (nonatomic, retain) GCDAsyncSocket* streamSocket;

@property (weak, nonatomic) IBOutlet UIButton *shiftButton;
@property (weak, nonatomic) IBOutlet UIButton *pedalButton;
@property (weak, nonatomic) IBOutlet UIImageView *streamImageView;
@property (weak, nonatomic) IBOutlet UILabel *waitingLabel;

@end
