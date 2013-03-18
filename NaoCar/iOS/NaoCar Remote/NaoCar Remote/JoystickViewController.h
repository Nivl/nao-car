//
//  JoystickViewController.h
//  NaoCar Remote
//
//  Created by gael on 15/03/13.
//  Copyright (c) 2013 NaoCar. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol JoystickViewControllerDelegate

-(void)joystick:(id)joystick valueChanged:(CGPoint)value;

@end

@interface JoystickViewController : UIViewController

-(void)updateJoystick;
-(void)hide;
-(void)show;

@property (weak, nonatomic) id<JoystickViewControllerDelegate> delegate;

@property (nonatomic) CGPoint value;
@property (nonatomic) CGPoint initialPosition;

@property (weak, nonatomic) IBOutlet UIImageView *button;
@property (weak, nonatomic) IBOutlet UIImageView *background;

@property (weak, nonatomic) UITouch* touch;

@end
