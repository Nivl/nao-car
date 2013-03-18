//
//  ActionsViewController.m
//  NaoCar Remote
//
//  Created by gael on 17/03/13.
//  Copyright (c) 2013 NaoCar. All rights reserved.
//

#import "ActionsViewController.h"

@implementation NSString (URLEncoding)
-(NSString *)urlEncodeUsingEncoding:(NSStringEncoding)encoding {
	return (NSString *)CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(NULL,
                                                               (CFStringRef)self,
                                                               NULL,
                                                               (CFStringRef)@"!*'\"();:@&=+$,/?%#[]% ",
                                                               CFStringConvertNSStringEncodingToEncoding(encoding)));
}
@end

@interface ActionsViewController ()

@end

@implementation ActionsViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    [self.controlsSelector setSelectedSegmentIndex:(int)_controlMode];
    [self.viewSelector setSelectedSegmentIndex:(int)_viewMode];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)changedView:(id)sender {
    UISegmentedControl* control = sender;
    [self.mainController sendCommand:[NSString stringWithFormat:@"change-view?view=%d", control.selectedSegmentIndex]];
    self.mainController.viewMode = (ViewMode)control.selectedSegmentIndex;
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField {
    NSString* message = textField.text;
    if (message.length != 0) {
        NSString* command = [NSString stringWithFormat:@"talk?message=%@", [message urlEncodeUsingEncoding:NSASCIIStringEncoding]];
        [self.mainController sendCommand:command];
        textField.text = @"";
    }
    return FALSE;
}

- (IBAction)launchAutoDriving:(id)sender {
    [self.mainController sendCommand:@"auto-driving"];
}

- (IBAction)funAction:(id)sender {
    [self.mainController sendCommand:@"fun-action"];
}

- (IBAction)carambarAction:(id)sender {
    [self.mainController sendCommand:@"carambar-action"];
}

- (IBAction)changedControl:(id)sender {
    UISegmentedControl* control = sender;
    self.mainController.controlMode = (ControlMode)control.selectedSegmentIndex;
}

- (IBAction)disabledNao:(id)sender {
    [self.mainController sendCommand:@"end"];
}

@end
