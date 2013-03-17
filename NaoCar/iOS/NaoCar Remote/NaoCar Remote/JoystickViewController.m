//
//  JoystickViewController.m
//  NaoCar Remote
//
//  Created by gael on 15/03/13.
//  Copyright (c) 2013 NaoCar. All rights reserved.
//

#import "JoystickViewController.h"

@interface JoystickViewController ()

@end

@implementation JoystickViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.button.alpha = 0.5;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)updateJoystick {
    self.button.center = [self.touch locationInView:self.view];
    
    CGPoint value = CGPointMake(self.button.center.x - self.background.center.x, self.button.center.y - self.background.center.y);
    float norm = sqrtf(value.x * value.x + value.y * value.y);
    float width = self.background.frame.size.width / 2;
    if (norm > width) {
        value = CGPointMake(value.x / norm, value.y / norm);
        self.button.center =
            CGPointMake(self.background.center.x + value.x * width,
                        self.background.center.y + value.y * width);
    } else {
        value = CGPointMake(value.x / width, value.y / width);
    }
    value.y = -value.y;
    self.value = value;
    if (self.delegate)
        [self.delegate joystick:self valueChanged:value];
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch* touch in touches) {
        self.touch = touch;
        self.background.center = [self.touch locationInView:self.view];
        [self updateJoystick];
        return ;
    }
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch* touch in touches) {
        if (touch == self.touch) {
            [self updateJoystick];
            return ;
        }
    }
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch* touch in touches) {
        if (touch == self.touch) {
            [UIView animateWithDuration:0.2 animations:^{
                self.button.center = self.background.center;
            }];
            self.value = CGPointMake(0, 0);
            if (self.delegate)
                [self.delegate joystick:self valueChanged:self.value];
            return ;
        }
    }
}

-(void)setInitialPosition:(CGPoint)initialPosition {
    _initialPosition = initialPosition;
    self.view.center = _initialPosition;
}

@end
