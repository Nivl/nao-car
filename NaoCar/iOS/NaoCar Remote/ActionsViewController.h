//
//  ActionsViewController.h
//  NaoCar Remote
//
//  Created by gael on 17/03/13.
//  Copyright (c) 2013 NaoCar. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "ViewController.h"

@interface ActionsViewController : UIViewController<UITextFieldDelegate>

- (IBAction)changedView:(id)sender;
- (IBAction)launchAutoDriving:(id)sender;
- (IBAction)funAction:(id)sender;
- (IBAction)carambarAction:(id)sender;
- (IBAction)changedControl:(id)sender;
- (IBAction)disabledNao:(id)sender;

@property (nonatomic, weak) ViewController* mainController;

@property (nonatomic) ControlMode controlMode;
@property (nonatomic) ViewMode viewMode;

@property (weak, nonatomic) IBOutlet UISegmentedControl *controlsSelector;
@property (weak, nonatomic) IBOutlet UISegmentedControl *viewSelector;

@end
